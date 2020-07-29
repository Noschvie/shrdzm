/*
  SHRDZMGateway

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 01 June 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#include "config/config.h"
#include <ArduinoJson.h>
#include "SimpleEspNowConnection.h"
#include "SetupObject.h"
#include "StringSplitter.h"
#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>

#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <PubSubClient.h>

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
DynamicJsonDocument configdoc(1024);
JsonObject configurationDevices  = configdoc.createNestedObject("devices");


SoftwareSerial SerialAT(14, 12); // RX, TX for SIM800
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

String MQTT_TOPIC;
String nodeName;
String localIP;
String inputString;
String clientAddress;
String myAddress;
String SSID;
String password;
String host;
String url;
bool firmwareUpdate = false;
String lastVersionNumber;
String currVersion;
String ver, nam;
uint32_t lastReconnectAttempt = 0;
String deviceName;

ESP8266WiFiMulti WiFiMulti;

const char apn[] = "WEPAPN";
const char gprsUser[] = "webapn.at";
const char gprsPass[] = "";

const char* broker = "xxx";
const int port = 8883;

String subcribeTopicSet;
String subcribeTopicConfig;

SetupObject setupObject;

void mqttCallback(char* topic, byte* payload, unsigned int len) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();

  ///// ..........
}

boolean mqttConnect() 
{
  Serial.print("Connecting to ");
  Serial.print(broker);


  // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect("GsmClientName", "xxx", "xxx");

  if (status == false) 
  {
    Serial.println(" fail");
    return false;
  }
  
  Serial.println(" success");
  mqtt.publish("SIM800", "GsmClientTest started");

  mqtt.subscribe(subcribeTopicSet.c_str());
  mqtt.subscribe(subcribeTopicConfig.c_str());
  
  return mqtt.connected();
}

String macToStr(const uint8_t* mac)
{
  char mac_addr[13];
  mac_addr[12] = 0;
  
  sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

  return String(mac_addr);
}

void setDeviceName()
{
  uint8_t pmac[6];
  WiFi.macAddress(pmac);
  deviceName = macToStr(pmac);

  deviceName.replace(":", "");
  deviceName.toUpperCase();
}

// for firmware upgrade
void update_started() 
{
  Serial.println("CALLBACK:  HTTP update process started");
  
  String s = "~000[U]$upgrade:started";
  Serial.write(s.c_str(), s.length());
  Serial.write('\n');
  delay(100);
}

void update_finished() 
{
  //Serial.println("CALLBACK: HTTP update process finished");

  String s = "~000[U]$upgrade:finished";
  Serial.write(s.c_str(), s.length());
  Serial.write('\n');
  delay(100);
}

void update_progress(int cur, int total) 
{
  //Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes.\n", cur, total);
}

void update_error(int err) 
{
  //Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);

  String s = "~000[U]$upgrade:error";
  Serial.write(s.c_str(), s.length());
  Serial.write('\n');
  delay(100);
}

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

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  if(String((char *)message) == "$PING$")
  {
    OnConnected(ad, simpleEspConnection.macToStr(ad));
    return;
  }
  
#ifdef DEBUG  
  Serial.println("MESSAGE:'"+String((char *)message)+"' from "+simpleEspConnection.macToStr(ad));
#endif

  String m = (char *)message;

  if(m.substring(0,3) == "$D$")       // Data
  {
    String s = "*000[D]$"+simpleEspConnection.macToStr(ad)+"$"+m.substring(3);
    Serial.write(s.c_str(), s.length());
    delay(100);
    Serial.print('\n');
    delay(100);  
  }
  else if((m.substring(0,4) == "$SC$") || // Device Configuration
          (m.substring(0,4) == "$SP$") || // Sensor Configuration
          (m.substring(0,4) == "$AP$") || // Action Configuration
          (m.substring(0,4) == "$SD$"))   // Parameter names
  {
    String prefix = (m.substring(0,4) == "$SC$" || m.substring(0,4) == "$SP$") ? "C" : "P";

    if(m.substring(0,4) == "$AP$")
      prefix = "A";
    
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
          String s = "*000["+prefix+"]$"+simpleEspConnection.macToStr(ad)+"$"+buffer.substring(f+1, nf);
          Serial.write(s.c_str(), s.length());
          Serial.print('\n');
          f = nf;
        }
        else
          run = false;
      }while(run);
  
      String s = "*000["+prefix+"]$"+simpleEspConnection.macToStr(ad)+"$"+buffer.substring(buffer.lastIndexOf('|')+1);
      Serial.write(s.c_str(), s.length());
      Serial.print('\n');
    }
    else
    {
      String s = "*000["+prefix+"]$"+simpleEspConnection.macToStr(ad)+"$"+buffer;
      Serial.write(s.c_str(), s.length());
      Serial.print('\n');
    }        
  }
  else if(m.substring(0,3) == "$V$")  // Version
  {
    String s = "*000[V]$"+simpleEspConnection.macToStr(ad)+"$"+m.substring(3);
    Serial.write(s.c_str(), s.length());
    Serial.print('\n');
  }
  else if(m.substring(0,3) == "$X$")  // Supported devices
  {
    String s = "*000[X]$"+simpleEspConnection.macToStr(ad)+"$"+m.substring(3);
    Serial.write(s.c_str(), s.length());
    Serial.print('\n');
  }
  else if(m.substring(0,3) == "$I$")  // Init due to new firmware
  {
    String s = "*000[I]$"+simpleEspConnection.macToStr(ad)+"$INIT";
    Serial.write(s.c_str(), s.length());
    Serial.print('\n');
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

  setupObject.AddItem(ad, "$SC$configuration");        

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
    String message = si->m_parameterName;

    if(si->m_parameterValue != "")
      message += ":"+ si->m_parameterValue;
    
    simpleEspConnection.sendMessage((char *)message.c_str(), ad);  

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

void storeLastVersionNumber()
{
  File file = SPIFFS.open("/version.txt", "w");
  if (!file) 
  {
      Serial.println("Error opening version file for writing");
      return;
  }  

  String v = ESP.getSketchMD5();

  int bytesWritten = file.write(v.c_str(), v.length());
   
  if (bytesWritten == 0) 
  {
      Serial.println("Version file write failed");
  }

  file.close();
}

void readLastVersionNumber()
{
  if (!(SPIFFS.exists ("/version.txt") ))
  {
    lastVersionNumber = "";
    return;
  }

  File file = SPIFFS.open("/version.txt", "r");

  lastVersionNumber= "";
    
  for(int i=0;i<file.size();i++) //Read upto complete file size
  {
    lastVersionNumber += (char)file.read();
  }

  file.close();  
}

bool initializeSIM800()
{
  MQTT_TOPIC = "SHRDZM/"+deviceName;
  nodeName = MQTT_TOPIC;

  subcribeTopicSet = String(MQTT_TOPIC)+"/set";
  subcribeTopicConfig = String(MQTT_TOPIC)+"/config/set";

  Serial.println("Will start modem...please wait.... ");
  SerialAT.begin(9600);
  delay(6000);

  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

/*  if ( GSM_PIN && modem.getSimStatus() != 3 ) 
  {
    modem.simUnlock(GSM_PIN);
  }*/


  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork()) 
  {
    Serial.println(" fail");
    return false;
  }
  
  Serial.println(" success");

  if (modem.isNetworkConnected()) 
  {
    Serial.println("Network connected");
  }

  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) 
  {
    Serial.println(" fail");
    return false;
  }

  Serial.println(" success");

  if (modem.isGprsConnected()) 
  {
    Serial.println("GPRS connected");
  }

  localIP = modem.getLocalIP();

  mqtt.setServer(broker, port);
  mqtt.setCallback(mqttCallback);

  return true;
}

void setup() 
{
  Serial.begin(9600);
  Serial.println();

#ifdef VERSION
  ver = String(VERSION);
#else
  ver = "0.0.0";  
#endif

#ifdef NAME
  nam = String(NAME);
#else
  nam = "SHRDZMGateway";  
#endif

  setDeviceName();

  initializeSIM800();
  

  SPIFFS.begin();

  readLastVersionNumber();
  currVersion = ESP.getSketchMD5();

#ifdef DEBUG
//  Serial.println("Will read config now... ");
  Serial.println("Last Version : "+lastVersionNumber+", Curr Version : "+currVersion);
#endif        
  if(!readConfig())
  {    
    writeConfig();
  }   

  if(strcmp(lastVersionNumber.c_str(), currVersion.c_str()) != 0)
  {
    // TODO
    storeLastVersionNumber();
  }

  uint8_t pmac[6];
  WiFi.macAddress(pmac);
  myAddress = simpleEspConnection.macToStr(pmac);

  String s = "~000[G]$address:"+myAddress;
  Serial.write(s.c_str(), s.length());
  Serial.write('\n');
  delay(100);

  s = "~000[V]$version:"+ver+"-"+currVersion;
  Serial.write(s.c_str(), s.length());
  Serial.write('\n');
  delay(100);

  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(2);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onPairingFinished(&OnPairingFinished);    
  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onConnected(&OnConnected);  

  setupObject.init(&simpleEspConnection);
}

void getConfig()
{
  String c; 

  serializeJson(configdoc, c);

  Serial.println("#"+c);    
}

void updateFirmware(String parameter)
{
  StringSplitter *splitter = new StringSplitter(parameter, '|', 4);
  int itemCount = splitter->getItemCount();

  if(itemCount == 3)
  {
    SSID = splitter->getItemAtIndex(0);
    password = splitter->getItemAtIndex(1);
    host = splitter->getItemAtIndex(2);

    if(host.substring(0,7) != "http://")
    {
      Serial.println("Upgrade : only http addresses supported!");
      return;    
    }    

    host = host.substring(7);
  
    if(host.substring(host.length()-4) != ".php")
    {
      Serial.println("Upgrade : only php update script supported");
      return;    
    }
  
    if(host.indexOf('/') == -1)
    {
      Serial.println("Upgrade : host string not valid");
      return;    
    }
  
    url = host.substring(host.indexOf('/'));
    host = host.substring(0,host.indexOf('/'));

    firmwareUpdate = true;

    esp_now_deinit();
    delay(100);
  
    WiFi.mode(WIFI_STA);
  
    WiFiMulti.addAP(SSID.c_str(), password.c_str());    
  
    //Serial.println("upgrade started with SSID:"+SSID+" password:"+password+" host:"+host);
  }
}

void loop() 
{
  if (!mqtt.connected()) 
  {
    Serial.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) 
    {
      lastReconnectAttempt = t;
      if (mqttConnect()) 
      {
        lastReconnectAttempt = 0;
        
        mqtt.publish((String(MQTT_TOPIC)+"/state").c_str(), "up");
        mqtt.publish((String(MQTT_TOPIC)+"/IP").c_str(), localIP.c_str());
  
  #ifdef VERSION
        mqtt.publish((String(MQTT_TOPIC)+"/version").c_str(), String(VERSION).c_str());
  #else      
        mqtt.publish((String(MQTT_TOPIC)+"/version").c_str(), "0.00");
  #endif
  
        mqtt.publish((String(MQTT_TOPIC)+"/gatewaymqttversion").c_str(), String(ver+"-"+currVersion).c_str());      
      }
    }
    delay(100);
    return;
  }

  mqtt.loop();
  
  simpleEspConnection.loop();

  if(firmwareUpdate)
  {
    if ((WiFiMulti.run() == WL_CONNECTED)) 
    {     
      ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);         

      // for firmware upgrade
      ESPhttpUpdate.onStart(update_started);
      ESPhttpUpdate.onEnd(update_finished);
      ESPhttpUpdate.onProgress(update_progress);
      ESPhttpUpdate.onError(update_error);  

      String versionStr = nam+" "+ver+" "+currVersion;
      Serial.println("WLAN connected! ");

      WiFiClient client;  
      t_httpUpdate_return ret = ESPhttpUpdate.update(host, 80, url, versionStr);    
      
      switch (ret) 
      {
        case HTTP_UPDATE_FAILED:
          {
            String s = "~000[U]$upgrade:failed";
            Serial.write(s.c_str(), s.length());
            Serial.write('\n');
            delay(100);
          
            ESP.restart();
          }
          break;
  
        case HTTP_UPDATE_NO_UPDATES:
          {
            String s = "~000[U]$upgrade:noupdate";
            Serial.write(s.c_str(), s.length());
            Serial.write('\n');
            delay(100);
  
            ESP.restart();
          }
          break;
  
        case HTTP_UPDATE_OK:
          {
            String s = "~000[U]$upgrade:done";
            Serial.write(s.c_str(), s.length());
            Serial.write('\n');
            delay(100);
          }
          break;
      }
    }
  }
  else
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
        else if(inputString.substring(0,9) == "$upgrade ") // Upgrade Gateway
        {
          StringSplitter *splitter = new StringSplitter(inputString, ' ', 4);
          int itemCount = splitter->getItemCount();
          
          if(itemCount == 2)
          {
            // SSID, password, host
            updateFirmware(splitter->getItemAtIndex(1));
          }        
        }
        else if(inputString.substring(0,5) == "$set ") // 
        {
          StringSplitter *splitter = new StringSplitter(inputString, ' ', 4);
          int itemCount = splitter->getItemCount();
  
          if(itemCount== 4 && splitter->getItemAtIndex(2) == "upgrade")  // Upgrade Sensor
          {
            String s = "$U$"+splitter->getItemAtIndex(3);
            setupObject.AddItem(splitter->getItemAtIndex(1), (char *)s.c_str());
          }
          else if(itemCount == 3)
          {
            setupObject.AddItem(splitter->getItemAtIndex(1), "$SC$"+splitter->getItemAtIndex(2));        
          }
          else if(itemCount == 4)
          {
            setupObject.AddItem(splitter->getItemAtIndex(1), "$SC$"+splitter->getItemAtIndex(2), splitter->getItemAtIndex(3));        
          }
        }                
        else if(inputString.substring(0,14) == "$configuration") // $configuration A4CF12D5D767 
        {
          StringSplitter *splitter = new StringSplitter(inputString, ' ', 4);
          String s = "$A$";
          setupObject.AddItem(splitter->getItemAtIndex(1), (char *)s.c_str());
        }                
        else if(inputString.substring(0,7) == "$pair") // 
        {
          simpleEspConnection.startPairing(30);
        }                
        else if(inputString.substring(0,4) == "test") // 
        {
          StringSplitter *splitter = new StringSplitter(inputString, ' ', 4);
          int itemCount = splitter->getItemCount();
  
          if(itemCount == 3)
          {
            setupObject.AddItem(splitter->getItemAtIndex(1), splitter->getItemAtIndex(2));        
          }
        }
        
        inputString = "";
      }
      else
      {
        inputString += inChar;
      }
    }
  }
}
