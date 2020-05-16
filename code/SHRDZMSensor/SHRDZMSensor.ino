/*
  SHRDZMSensor

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 15 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

// #define DISABLEGOTOSLEEP

#if defined(ESP8266)
#include <FS.H>
#endif
#include <ArduinoJson.h>
#include "config/config.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>

#include "SimpleEspNowConnection.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);
DynamicJsonDocument configdoc(1024);
JsonObject configuration = configdoc.createNestedObject("configuration");
JsonObject device_configuration = configuration.createNestedObject("device");

String inputString;
String serverAddress;
unsigned long clockmillis = 0;
volatile bool pairingOngoing = false;
DeviceBase* dev;
bool deviceTypeSet = true;
bool gatewayMessageDone = false;
long setupMessageHandled = 0;
int sendQueue = 0;

ESP8266WiFiMulti WiFiMulti;
String SSID;
String password;
String host;
String url;
bool firmwareUpdate = false;

// for firmware upgrade
void update_started() 
{
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() 
{
  Serial.println("CALLBACK:  HTTP update process finished");

  pairingOngoing = false;
}

void update_progress(int cur, int total) 
{
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) 
{
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);

  pairingOngoing = false;
}

bool readConfig()
{
    if (SPIFFS.exists("/shrdzm_config.json")) 
    {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/shrdzm_config.json", "r");
      if (configFile) 
      {
        Serial.println("opened config file");
        // Allocate a buffer to store contents of the file.

        String content;
        
        for(int i=0;i<configFile.size();i++) //Read upto complete file size
        {
          content += (char)configFile.read();
        }

#ifdef DEBUG
        Serial.println(content);
#endif

        DeserializationError error = deserializeJson(configdoc, content);
        if (error)
        {
          Serial.println("Error at deserializeJson");
      
          return false;
        }

        configFile.close();
      }
    }
    else
    {
      Serial.println("shrdzm_config.json does not exist");
      return false;
    }
}

bool writeConfig()
{
    File configFile = SPIFFS.open("/shrdzm_config.json", "w");
    if (!configFile) 
    {
      Serial.println("failed to open config file for writing");
      return false;
    }

    serializeJson(configdoc, configFile);
    configFile.close();
}

void sendSetup()
{
    String reply;
    
    // send device setup
    reply = "$SC$";
    
    for (JsonPair kv : configuration) 
    {
      if(String(kv.key().c_str()) != "device")
        reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";
    }

    reply.remove(reply.length()-1);
    
    simpleEspConnection.sendMessage((char *)reply.c_str());

    // send device parameter
    if(configuration["device"].size() > 0)
    {
      reply = "$SP$";
  
      JsonObject sp = configuration["device"];
      for (JsonPair kv : sp) 
      {
        reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";
      }
  
      reply.remove(reply.length()-1);
      simpleEspConnection.sendMessage((char *)reply.c_str());
    }
    
    // send sensor parameter types
    SensorData* sd = dev->readParameterTypes();

    if(sd != NULL)
    {
      reply = "$SD$";

      for(int i = 0; i<sd->size; i++)
      {
        reply += sd->di[i].nameI;
        if(i < sd->size-1)
          reply += "|";
      }

      simpleEspConnection.sendMessage((char *)reply.c_str());
    }

    delete sd;  

    // send firmware version
    String s = String("$V$")+ESP.getSketchMD5();
    simpleEspConnection.sendMessage((char *)s.c_str());    
}

void updateFirmware(String message)
{
  if(message.indexOf('|') == -1)
    return;

  SSID = getValue(message, '|', 0);
  password = getValue(message, '|', 1);
  host = getValue(message, '|', 2);

  if(host.substring(0,7) != "http://")
  {
    Serial.println("Upgrade : only http address supported!");
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
  
//  Serial.println("SSID:"+SSID+" Password:"+password+" Host:"+host+" url:"+url);

  pairingOngoing = true;
  firmwareUpdate = true;   

  sendQueue++;
  simpleEspConnection.sendMessage((char *)String("$FW$"+ESP.getSketchMD5()).c_str());

  delay(100);
  
  esp_now_deinit();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID.c_str(), password.c_str());  
}

void OnMessage(uint8_t* ad, const char* message)
{
  Serial.println("MESSAGE:"+String(message));

  if(String(message) == "$SLEEP$") // force to go sleep
  {
    gatewayMessageDone = true;
    Serial.println("FORCE SLEEP MODE");
    
    return;
  }
  else if(String(message).substring(0,3) == "$U$") // update firmware
  {
    pairingOngoing = true;

    updateFirmware(String(message).substring(3));

    pairingOngoing = false;        
  }
  else if(String(message) == "$S$") // ask for settings
  {
    pairingOngoing = true;

    if(deviceTypeSet)
    {
      sendSetup();
    }
    else
    {
      Serial.println("Gateway asked for settings but I did'nt sent because device type is not set!");    
    }

    pairingOngoing = false;    
  }
  else if(String(message).substring(0,5) == "$SDT$") // set device type
  {
    pairingOngoing = true;

    setDeviceType(String(message).substring(5));

    pairingOngoing = false;    
  }
  else if(String(message).substring(0,4) == "$SC$") // set configuration
  {
    pairingOngoing = true;

    setConfig(String(message).substring(4));

    pairingOngoing = false;    
  }

  //gatewayMessageDone = true;
  setupMessageHandled = millis()+300;
}

void OnPairingFinished()
{
  Serial.println("Pairing finished");
  pairingOngoing = false;  
  clockmillis = millis();  
}

void OnNewGatewayAddress(uint8_t *ga, String ad)
{  
  Serial.println("New GatewayAddress '"+ad+"'");

  simpleEspConnection.setServerMac(ga);
  configuration["gateway"] = ad;  

  writeConfig();    
}

void OnSendError(uint8_t* ad)
{
  sendQueue--;

  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE! - sendQueue = "+String(sendQueue));

  pairingOngoing = false;
}

void OnSendDone(uint8_t* ad)
{
  if(sendQueue > 0)
    sendQueue--;

#ifdef DEBUG
  Serial.println("Sending to '"+simpleEspConnection.macToStr(ad)+"' done - sendQueue = "+String(sendQueue));
#endif    
}

void setup() 
{
#ifdef DEBUG
  Serial.begin(9600); Serial.println();
  Serial.println(ESP.getSketchMD5());
#endif



  SPIFFS.begin();

  if(!readConfig())
  {
    SPIFFS.format();

    int i = SLEEP_SECS;
    configuration["interval"] = String(i);

    int s = SENSORPOWERPIN;
    configuration["sensorpowerpin"] = String(s);

    s = PAIRING_PIN;
    configuration["pairingpin"] = String(s);

    writeConfig();    
  }  

  pinMode(configuration["sensorpowerpin"].as<uint8_t>(), INPUT_PULLUP);

  simpleEspConnection.begin();
  simpleEspConnection.onPairingFinished(&OnPairingFinished);  
  simpleEspConnection.setPairingBlinkPort(LEDPIN);  
  simpleEspConnection.onSendError(&OnSendError);    
  simpleEspConnection.onSendDone(&OnSendDone);
  if(configuration.containsKey("gateway"))
  {
    simpleEspConnection.setServerMac(configuration["gateway"].as<String>());  
  }

  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onMessage(&OnMessage);  

  ///////////////////
  

  if(digitalRead(configuration["pairingpin"].as<uint8_t>()) == false)
  {
    pairingOngoing = true;
    simpleEspConnection.startPairing(300);
  }
  else
  {
    pinMode(configuration["sensorpowerpin"].as<uint8_t>(),OUTPUT);
    digitalWrite(configuration["sensorpowerpin"].as<uint8_t>(),HIGH);    

    // setup the device
    if(configuration["devicetype"] == "DHT22")
    {
      dev = new Device_DHT22();
    }
    else if(configuration["devicetype"] == "BH1750")
    {
      dev = new Device_BH1750();
    }
    else if(configuration["devicetype"] == "DS18B20")
    {
      dev = new Device_DS18B20();
    }
    else if(configuration["devicetype"] == "HTU21D")
    {
      dev = new Device_HTU21D();
    }
    else if(configuration["devicetype"] == "WATERSENSOR")
    {
      dev = new Device_Watersensor();
    }

    if(dev != NULL)
    {
      pairingOngoing = true;
      
      dev->setDeviceParameter(configuration["device"]);
  
      SensorData* sd = dev->readParameter();
  
      if(sd != NULL)
      {
        String reply;
        
        for(int i = 0; i<sd->size; i++)
        {
          reply = "$D$";
  
          reply += sd->di[i].nameI+":"+sd->di[i].valueI;

          sendQueue++;
          simpleEspConnection.sendMessage((char *)reply.c_str());
        }
      }
      else
      {
        deviceTypeSet = false;
      }
  
      delete sd;      
      
      pairingOngoing = false;      
    }
    else
    {
      Serial.println("Will not work until device type is set!");
    }

    // for firmware upgrade
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
    
  }

  clockmillis = millis();
}

void setConfig(String cmd)
{
  if(cmd.indexOf(':') == -1)
    return;

  String pname = getValue(cmd, ':', 0);
  String pvalue = getValue(cmd, ':', 1);

   Serial.println("setConfig "+pvalue);

  if( pname == "interval" || 
      pname == "sensorpowerpin" || 
      pname == "devicetype" || 
      pname == "gateway")
  {
    if(pname == "devicetype")
    {
      setDeviceType(pvalue);
    }
    else
    {
      configuration[pname] = pvalue;
       
      writeConfig();    
      sendSetup();     
    }
  }
  else
  {
    if(configuration["device"].containsKey(pname))
    {
      configuration["device"][pname] = pvalue;
       
      writeConfig();    
      sendSetup();     
    }
  }
}

void setDeviceType(String deviceType)
{
  if(deviceType == "DHT22" ||
     deviceType == "BH1750" ||
     deviceType == "DS18B20" ||
     deviceType == "HTU21D" ||
     deviceType == "WATERSENSOR")
  {
    configuration["devicetype"] = deviceType;

   Serial.println("setDeviceType "+deviceType);


    if(configuration.containsKey("device"))
    {
      configuration.remove("device");
    }

    delete dev;

    if(deviceType == "DHT22")
    {
      dev = new Device_DHT22();
    }
    else if(deviceType == "BH1750")
    {
      dev = new Device_BH1750();
    }
    else if(deviceType == "DS18B20")
    {
      dev = new Device_DS18B20();
    }
    else if(deviceType == "HTU21D")
    {
      dev = new Device_HTU21D();
    }
    else if(deviceType == "WATERSENSOR")
    {
      dev = new Device_Watersensor();
    }
    else
    {
      deviceTypeSet = false;
      return;
    }

    dev->initialize();
    
    JsonObject dc = dev->getDeviceParameter();
    configuration["device"] = dc;
      
    writeConfig();    
    
    sendSetup();
  }  
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++)
  {
    if(data.charAt(i)==separator || i==maxIndex)
    {
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void initialization()
{
  // format 
  SPIFFS.format();
  
  delay(100);
  ESP.restart();      
}

void sendInfo()
{
  Serial.println("pairingOngoing : "+String(pairingOngoing));
  Serial.println("gatewayMessageDone : "+String(gatewayMessageDone));
  Serial.println("firmwareUpdate : "+String(firmwareUpdate));
  Serial.println("sendQueue : "+String(sendQueue));
  
}

void loop() 
{
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') 
    {
      Serial.println(inputString);

      if(inputString == "startpair")
      {
        simpleEspConnection.startPairing(30);
      }
      else if(inputString.substring(0,14) == "setdevicetype ")
      {
        setDeviceType(inputString.substring(14));
      }    
      else if(inputString == "endpair")
      {
        simpleEspConnection.endPairing();
      }      
      else if(inputString == "init")
      {
        initialization();
      }      
      else if(inputString == "info")
      {
        sendInfo();
      }      
      else if(inputString == "sendtest")
      {
         simpleEspConnection.sendMessage("This comes from the Client");
      }
      
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }

  if(firmwareUpdate)
  {    
    pairingOngoing = true;
    if ((WiFiMulti.run() == WL_CONNECTED)) 
    {     
      firmwareUpdate = false;
      
      ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);         

      String versionStr = ESP.getSketchMD5();
      Serial.println("WLAN connected!");

      WiFiClient client;  
      t_httpUpdate_return ret = ESPhttpUpdate.update(host, 80, url, versionStr);    
      
      switch (ret) 
      {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;
  
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
  
        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      }

      pairingOngoing = false;
      sendQueue = 0;
    }
  }

#ifndef DISABLEGOTOSLEEP    
  if (((millis() > MAXCONTROLWAIT+clockmillis 
      && millis() > setupMessageHandled
      && !pairingOngoing) ||
      gatewayMessageDone) &&
      !firmwareUpdate && sendQueue == 0
      )
  {
    // everything donw and I can go to sleep
    gotoSleep();
  }
#endif    
}

void gotoSleep() 
{  
  delete dev;
  
  int sleepSecs = configuration["interval"]; 
#ifdef DEBUG
  Serial.printf("Up for %i ms, going to sleep for %i secs...\n", millis(), sleepSecs); 
#endif

  ESP.deepSleep(sleepSecs * 1000000, RF_NO_CAL);
  delay(100);
}
