/*
  SHRDZMGateway

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 15 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#include "config/config.h"
#include <ArduinoJson.h>
#include "SimpleEspNowConnection.h"
#include "SetupObject.h"
#include "StringSplitter.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
DynamicJsonDocument configdoc(1024);
JsonObject configurationDevices  = configdoc.createNestedObject("devices");

String inputString;
String clientAddress;

SetupObject setupObject;

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

void OnSendError(uint8_t* ad)
{
#ifdef DEBUG  
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
#endif  
}

void OnMessage(uint8_t* ad, const char* message)
{
#ifdef DEBUG  
  Serial.println("MESSAGE:'"+String(message)+"' from "+simpleEspConnection.macToStr(ad));
#endif

  String m = message;

  if(m.substring(0,3) == "$D$")       // Data
  {
    String s = "*000[D]$"+simpleEspConnection.macToStr(ad)+"$"+m.substring(3);
    Serial.write(s.c_str(), s.length());
    delay(100);
    Serial.print('\n');
    delay(100);  
  }
  else if(m.substring(0,4) == "$SC$")  // Configuration
  {
    String buffer = m.substring(4);
    int f = -1;
    bool run = true;

    if(buffer.indexOf('|') > -1)
    {
      do
      {
        int nf = buffer.indexOf('|', f+1);
        if(nf > 0)
        {
          String s = "*000[C]$"+simpleEspConnection.macToStr(ad)+"$"+buffer.substring(f+1, nf);
          Serial.write(s.c_str(), s.length());
          Serial.print('\n');
          f = nf;
        }
        else
          run = false;
      }while(run);
  
      String s = "*000[C]$"+simpleEspConnection.macToStr(ad)+"$"+buffer.substring(buffer.lastIndexOf('|')+1);
      Serial.write(s.c_str(), s.length());
      Serial.print('\n');
    }
    else
    {
      String s = "*000[C]$"+simpleEspConnection.macToStr(ad)+"$"+buffer;
      Serial.write(s.c_str(), s.length());
      Serial.print('\n');
    }
    
    
  }
  else
  {
    String s = "*000[E]$"+simpleEspConnection.macToStr(ad)+"$"+m;
    Serial.write(s.c_str(), s.length());
    delay(100);
    Serial.print('\n');
    delay(100);  
  }
}

void OnPaired(uint8_t *ga, String ad)
{
#ifdef DEBUG
  Serial.println("EspNowConnection : Client '"+ad+"' paired! ");
#endif

  if(!configurationDevices.containsKey(ad))
  {
    JsonObject newDevice  = configurationDevices.createNestedObject(ad);
    
    writeConfig();

  }
  
  String s = "*000[P]$"+ad+"$paired:OK";
  Serial.write(s.c_str(), s.length());
  delay(100);
  Serial.print('\n');
  delay(100);  

  // get all possible parameter
  simpleEspConnection.sendMessage("$S$", ad);    
}

void OnConnected(uint8_t *ga, String ad)
{
#ifdef DEBUG
  Serial.println("EspNowConnection : Client '"+ad+"' connected! ");
#endif

  clientAddress = ad;
  SetupObject::SetupItem *si = setupObject.GetItem(ad);

  if(si != NULL)
  {
    simpleEspConnection.sendMessage((char *)si->m_parameterName.c_str(), ad);  

    setupObject.RemoveItem(si);              
  }  
  else
  {
    simpleEspConnection.sendMessage("$SLEEP$", ad);        
  }
}

void OnPairingFinished()
{
#ifdef DEBUG  
  Serial.println("Pairing finished");
#endif  
}

void setup() 
{
  Serial.begin(9600);
  Serial.println();
   //clientAddress = "807D3ADC8EF0"; // Test if you know the client

  SPIFFS.begin();

#ifdef DEBUG
    Serial.println("Will read config now...");
#endif        
  if(!readConfig())
  {    
    writeConfig();
  }   

  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(2);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onPairingFinished(&OnPairingFinished);    
  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onConnected(&OnConnected);  

#ifdef DEBUG
  Serial.println(WiFi.macAddress());    
#endif  
}

void getConfig()
{
  String c; 

  serializeJson(configdoc, c);

  Serial.println("#"+c);    
}

void loop() 
{
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') 
    {
#ifdef DEBUG      
      Serial.println(inputString);
#endif      
      if(inputString == "reset")  // reset me
      {
#ifdef DEBUG
        Serial.println("Reset..");
#endif        
        ESP.restart();      
      }
      else if(inputString == "$deleteconfig")  // delete my config
      {
        deleteConfig();
      }
      else if(inputString == "$getconfig")  // get config
      {
        getConfig();
      }
      else if(inputString == "$startpair")
      {
        simpleEspConnection.startPairing(30);
      }
      else if(inputString == "$endpair")
      {
        simpleEspConnection.endPairing();
      }
      else if(inputString == "$changepairingmac")
      {
        uint8_t np[] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};
        
        simpleEspConnection.setPairingMac(np);
      }      
      else if(inputString.substring(0,9) == "sendtest ")
      {
        simpleEspConnection.sendMessage((char *)inputString.substring(9).c_str(), clientAddress);
      }          
      else if(inputString.substring(0,9) == "posttest ")
      {
        setupObject.AddItem(clientAddress, inputString.substring(9));
      }          
      else if(inputString.substring(0,5) == "$set ") // 
      {
        StringSplitter *splitter = new StringSplitter(inputString, ' ', 4);
        int itemCount = splitter->getItemCount();

        if(itemCount == 3)
        {
          setupObject.AddItem(splitter->getItemAtIndex(1), "$SC$"+splitter->getItemAtIndex(2));        
        }
      }                
      else if(inputString.substring(0,7) == "$pair") // 
      {
        simpleEspConnection.startPairing(30);
      }                
      
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}
