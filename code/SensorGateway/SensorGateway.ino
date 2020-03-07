#include <FS.H>
#include <ArduinoJson.h>
#include "config/config.h"
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include "Ticker.h"
#include "StringSplitter.h"

extern "C" {
#include <espnow.h>
#include <user_interface.h>
}

// ESPNOW STUFF
uint8_t trigMac[6] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};
uint8_t broadcastMac[] = {0x46, 0x33, 0x33, 0x33, 0x33, 0x33};

uint8_t gatewayMac[6];

uint8_t key[16] = {0x51, 0xA0, 0xDE, 0xC5, 0x46, 0xC6, 0x77, 0xCD,
                   0x99, 0xE8, 0x61, 0xF9, 0x08, 0x77, 0x7D, 0x00
                  };


// contains one item to send to a specific device
class SetupObject
{
  public:
  class SetupItem
  {
    public:
    String m_deviceName;
    String m_parameterName;
    String m_parameterValue;

    public:
    SetupItem(String deviceName, String parameterName, String parameterValue)
    {
      m_deviceName = deviceName;
      m_parameterName = parameterName;
      m_parameterValue = parameterValue;
    };
  };

  public:
  SetupItem *items[20];
  
  void AddItem( String deviceName, String parameterName, String parameterValue )
  {
    for(int i = 0; i<20; i++)
    {
      if(items[i] == NULL)
      {
        items[i] = new SetupItem(deviceName, parameterName, parameterValue);
        break;
      }
    }
  };
  
  void AddInitItem( String deviceName )
  {
    for(int i = 0; i<20; i++)
    {
      if(items[i] == NULL)
      {
        items[i] = new SetupItem(deviceName, "init", "");
        break;
      }
    }
  };
  
  SetupItem* GetItem( String deviceName )
  {
    SetupItem *it = NULL;
    
    for(int i = 0;i<20; i++)
    {
      if(items[i] != NULL)
      {
        if(items[i]->m_deviceName == deviceName)
        {
          it = items[i];
        }
        break;
      }
    }

    return it;
  };  

  void RemoveItem(SetupItem *it)
  {
    for(int i = 0;i<20; i++)
    {
      if(items[i] != NULL && items[i] == it)
      {
        delete items[i];
        items[i] = NULL;
        break;
      }
    }
  };
};

SetupObject setupObject;

Ticker ticker, pairingTickerBlink;
int pairingCount = 0;
String deviceName;
volatile bool pairingOngoing = false;
DynamicJsonDocument configdoc(1024);
JsonObject configurationDevices  = configdoc.createNestedObject("devices");

bool deleteConfig()
{
  configdoc.clear();
  configurationDevices = configdoc.createNestedObject("devices");

  return writeConfig();
}

bool readConfig()
{
    if (SPIFFS.exists("/shrdzm_config.json")) 
    {
      //file exists, reading and loading
      File configFile = SPIFFS.open("/shrdzm_config.json", "r");
      if (configFile) 
      {
        // Allocate a buffer to store contents of the file.

        String content;
        
        for(int i=0;i<configFile.size();i++) //Read upto complete file size
        {
          content += (char)configFile.read();
        }

        DeserializationError error = deserializeJson(configdoc, content);
        if (error)
        {
#ifdef DEBUG
          Serial.println("Error at deserializeJson");
#endif      
          return false;
        }

        configFile.close();
      }
    }
    else
    {
#ifdef DEBUG
      Serial.println("shrdzm_config.json does not exist");
#endif
      return false;
    }

#ifdef DEBUG
    serializeJson(configdoc, Serial);
    Serial.println();
#endif    
}

bool writeConfig()
{
    File configFile = SPIFFS.open("/shrdzm_config.json", "w");
    if (!configFile) 
    {
#ifdef DEBUG
      Serial.println("failed to open config file for writing");
#endif
      return false;
    }

    serializeJson(configdoc, configFile);

#ifdef DEBUG
    serializeJson(configdoc, Serial);
    Serial.println();
#endif
    
    configFile.close();
}

void pairingTickerLED()
{
  if(pairingOngoing)
  {
    int state = digitalRead(LEDPIN);
    digitalWrite(LEDPIN, !state); 
  }
  else
  {
    digitalWrite(LEDPIN, HIGH);
    pairingTickerBlink.detach();
  }
}

void pairingTicker()
{
#ifdef DEBUG
    Serial.println("Pairing request sent...");
#endif

    String pl = "PAIRWITHME";

    uint8_t bs[pl.length()];
    memcpy(bs, pl.c_str(), sizeof(bs));
    esp_now_send(broadcastMac, bs, sizeof(bs));

    pairingCount++;
    if(pairingCount == 12)
    {
      Serial.println("*034[I]$"+deviceName+"$pairing:finished");
      pairingOngoing = false;
#ifdef DEBUG
      Serial.println("Pairing finished...");
#endif
      ticker.detach();
    }
}

void initVariant() 
{
  WiFi.mode(WIFI_AP);

  WiFi.macAddress(gatewayMac);

  gatewayMac[0] = 0xCE;
  gatewayMac[1] = 0x50;
  gatewayMac[2] = 0xE3;
  
  wifi_set_macaddr(SOFTAP_IF, &gatewayMac[0]);//new mac is set
}

void setup() 
{  
  Serial.begin(9600);
  Serial.println();
  SPIFFS.begin();


#ifdef DEBUG
    Serial.println("Will read config now...");
#endif        
  if(!readConfig())
  {    
    writeConfig();
  }


  WiFi.macAddress(gatewayMac);

  deviceName = macToStr(gatewayMac);
  deviceName.replace(":", "");
  deviceName.toUpperCase();

  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(&broadcastMac[0], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(&trigMac[0], ESP_NOW_ROLE_CONTROLLER, 1, &key[0], 16);
  esp_now_set_peer_key(&trigMac[0], &key[0], 16);
  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
  {
    String str((char*)data);
    bool forwardData = true;

    StringSplitter *splitter = new StringSplitter(str, '$', 3);
    int itemCount = splitter->getItemCount();

    if(configurationDevices.containsKey(splitter->getItemAtIndex(1)))
    {
      if(str.substring(4,7) == "[S]")
      {
        forwardData = false;
        
        if(itemCount == 3)
        {
          SetupObject::SetupItem *si = setupObject.GetItem(splitter->getItemAtIndex(1));
  
          if(si != NULL)
          {
            if(si->m_parameterName == "init")
            {
  #ifdef DEBUG
              Serial.println("Have to initialize "+splitter->getItemAtIndex(1));
  #endif
              String setupText = "I%"+splitter->getItemAtIndex(1)+"%init";

              uint8_t bs[setupText.length()];
              memcpy(bs, setupText.c_str(), sizeof(bs));
              esp_now_send(mac, bs, sizeof(bs));           
            }
            else
            {
  #ifdef DEBUG
              Serial.println("Have to set the value "+si->m_parameterValue+" for parameter "+
                si->m_parameterName);
  #endif
              String setupText = "S%"+splitter->getItemAtIndex(1)+"%"+
                    si->m_parameterName+":"+
                    si->m_parameterValue;
      
              uint8_t bs[setupText.length()];
              memcpy(bs, setupText.c_str(), sizeof(bs));
              esp_now_send(mac, bs, sizeof(bs));           
            }
            
            setupObject.RemoveItem(si);              
          }
        }      
      } 
  
      if(str.substring(4,7) == "[C]") // new device paired
      {
        if(itemCount == 3)
        {
          JsonObject newDevice;
          
          if(!configurationDevices.containsKey(splitter->getItemAtIndex(1)))
          {
            // create new entry
            newDevice  = configurationDevices.createNestedObject(splitter->getItemAtIndex(1));
          }
          else
          {
            newDevice  = configurationDevices[splitter->getItemAtIndex(1)];          
          }
  
          String v = splitter->getItemAtIndex(2).substring(splitter->getItemAtIndex(2).indexOf(':')+1);
          String t = splitter->getItemAtIndex(2).substring(0, splitter->getItemAtIndex(2).indexOf(':'));          
  
          newDevice[t] = v;
  
          writeConfig();
        }      
      } 
          
      if(str.substring(4,7) == "[D]") // data arrived
      {
      }

      if(forwardData)
      {
        Serial.write(data, len);
        delay(100);
        Serial.print('\n');
      }
      
      delay(100);
    }
    else
    {
      if(str.substring(4,7) == "[P]") // new device paired
      {
        if(itemCount == 3)
        {
          if(!configurationDevices.containsKey(splitter->getItemAtIndex(1)))
          {
            // create new entry
            JsonObject newDevice  = configurationDevices.createNestedObject(splitter->getItemAtIndex(1));
            
            writeConfig();
            
            Serial.write(data, len);
            delay(100);
            Serial.print('\n');
            delay(100);            
          }
        }            
      } 
      else
      {      
        Serial.println("*144[E]$"+deviceName+"$error:data from unpaired device "+splitter->getItemAtIndex(1));
      }        
    }

    
    delete splitter;
    });
}

void loop() 
{
  if (Serial.available() > 0)
  {
    if ( Serial.read() == '$' ) 
    {
      String cmd = readSerial();

      if(cmd == "reset")  // reset me
      {
#ifdef DEBUG
        Serial.println("Reset..");
#endif        
        ESP.restart();      
      }
      else if(cmd == "deleteconfig")  // delete my config
      {
        deleteConfig();
      }
      else if(cmd == "pair")  // pair with device
      {
        Serial.println("*033[I]$"+deviceName+"$pairing:started");

#ifdef DEBUG
        Serial.println("Pairing running...");
#endif        
        pairingCount = 0;
        pinMode(LEDPIN, OUTPUT);
        pairingOngoing = true;

        ticker.attach(5.0, pairingTicker);            
        pairingTickerBlink.attach(1.0, pairingTickerLED);            
      }
      else
      {
        StringSplitter *splitter = new StringSplitter(cmd, ' ', 4);
        int itemCount = splitter->getItemCount();

        if(itemCount == 4)
        {
          if(splitter->getItemAtIndex(0) == "set")
          {
            if(configurationDevices.containsKey(splitter->getItemAtIndex(1)))
            {
#ifdef DEBUG
              Serial.println("Set "+splitter->getItemAtIndex(2)+" to "+splitter->getItemAtIndex(3));
#endif
              setupObject.AddItem(splitter->getItemAtIndex(1), splitter->getItemAtIndex(2), splitter->getItemAtIndex(3));
            }
            else
            {
              Serial.println("*080[E]$"+deviceName+"$error:"+splitter->getItemAtIndex(1)+" not paired");
            }
          }
        }
        else if(itemCount == 2)
        {
          if(splitter->getItemAtIndex(0) == "init")
          {

            if(configurationDevices.containsKey(splitter->getItemAtIndex(1)))
            {
#ifdef DEBUG
              Serial.println("Init "+splitter->getItemAtIndex(1)+" as soon as available");
#endif
              setupObject.AddInitItem(splitter->getItemAtIndex(1));
            }          
            else
            {
              Serial.println("*080[E]$"+deviceName+"$error:"+splitter->getItemAtIndex(1)+" not paired");              
            }
          }
        }
        delete splitter;
      }
    }
  }
}

String readSerial() 
{
  String cmd = "";
  byte inByte = 0;
  int counter = 0;
  bool finished = false;

  while (!finished)
  {
    while (Serial.available() == 0 && counter < 20) 
    { 
      delay(1); 
      counter++;
    }
    if(counter == 20)
    {
      finished = true;
    }
    else
    {
      counter = 0;
      inByte = Serial.read();
      
      if (inByte == '\n')
      {
        finished = true;
      }
      else
      {
        cmd += (char)inByte;
      }
    }    
  }
      
  #ifdef DEBUG
  Serial.println("");  
  Serial.println("entered:"+cmd);  
  #endif     

  return cmd;
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

String split(String s, char parser, int index) 
{
  String rs="";
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) 
  {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) 
    {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex,rToIndex);
    }
    else 
      parserCnt++;
  }
  return rs;
}
