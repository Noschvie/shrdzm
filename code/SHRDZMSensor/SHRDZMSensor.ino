/*
  SHRDZMSensor

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 05 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

 #define DISABLEGOTOSLEEP

#include <FS.H>
#include <ArduinoJson.h>
#include "config/config.h"

#include "SimpleEspNowConnection.h"

#include "SIMPLEESPNOWCONNECTION_DHT22.h"
#include "SIMPLEESPNOWCONNECTION_BH1750.h"
#include "SIMPLEESPNOWCONNECTION_DS18B20.h"


SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);
DynamicJsonDocument configdoc(1024);
JsonObject configuration = configdoc.createNestedObject("configuration");
JsonObject device_configuration = configuration.createNestedObject("device");

String inputString;
String serverAddress;
unsigned long clockmillis = 0;
volatile bool pairingOngoing = false;
DeviceBase* dev;

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
}

void OnMessage(uint8_t* ad, const char* message)
{
  Serial.println("MESSAGE:"+String(message));

  if(String(message) == "$S$") // ask for settings
  {
    pairingOngoing = true;

    sendSetup();

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

void setup() 
{
#ifdef DEBUG
  Serial.begin(9600); Serial.println();
#endif

  SPIFFS.begin();

  if(!readConfig())
  {
    SPIFFS.format();

    int i = SLEEP_SECS;
    configuration["interval"] = String(i);

    int s = SENSORPOWERPIN;
    configuration["sensorpowerpin"] = String(s);

    writeConfig();    
  }  

  pinMode(PAIRING_PIN, INPUT_PULLUP);

  simpleEspConnection.begin();
  simpleEspConnection.onPairingFinished(&OnPairingFinished);  
  simpleEspConnection.setPairingBlinkPort(LEDPIN);  
  if(configuration.containsKey("gateway"))
  {
    simpleEspConnection.setServerMac(configuration["gateway"].as<String>());  
  }

  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onMessage(&OnMessage);  

  ///////////////////
  

  if(digitalRead(PAIRING_PIN) == false)
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
      dev = new SIMPLEESPNOWCONNECTION_DHT22();
    }
    else if(configuration["devicetype"] == "BH1750")
    {
      dev = new SIMPLEESPNOWCONNECTION_BH1750();
    }
    else if(configuration["devicetype"] == "DS18B20")
    {
      dev = new SIMPLEESPNOWCONNECTION_DS18B20();
    }

    dev->setDeviceParameter(configuration["device"]);

    SensorData* sd = dev->readParameter();

    if(sd != NULL)
    {
      String reply;
      
      for(int i = 0; i<sd->size; i++)
      {
        reply = "$D$";

        reply += sd->di[i].nameI+":"+sd->di[i].valueI;
        
        simpleEspConnection.sendMessage((char *)reply.c_str());
      }
    }

    delete sd;      
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
      dev = new SIMPLEESPNOWCONNECTION_DHT22();
    }
    else if(deviceType == "BH1750")
    {
      dev = new SIMPLEESPNOWCONNECTION_BH1750();
    }
    else if(deviceType == "DS18B20")
    {
      dev = new SIMPLEESPNOWCONNECTION_DS18B20();
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

#ifndef DISABLEGOTOSLEEP    
  if (millis() > MAXCONTROLWAIT+clockmillis && !pairingOngoing) 
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
