/*
  SHRDZMSensor

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 05 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

// #define DISABLEGOTOSLEEP

#include <FS.H>
#include <ArduinoJson.h>
#include "config/config.h"

#include "SimpleEspNowConnection.h"

#include "SIMPLEESPNOWCONNECTION_DHT22.h"


SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);
DynamicJsonDocument configdoc(1024);
JsonObject configuration = configdoc.createNestedObject("configuration");
JsonObject device_configuration = configuration.createNestedObject("device");

String inputString;
String serverAddress;
unsigned long clockmillis = 0;
volatile bool pairingOngoing = false;

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

void OnMessage(uint8_t* ad, const char* message)
{
  Serial.println("MESSAGE:"+String(message));
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

  if(digitalRead(PAIRING_PIN) == false)
  {
    pairingOngoing = true;
    simpleEspConnection.startPairing(300);
  }
  else
  {
    pinMode(SENSORPOWERPIN,OUTPUT);
    digitalWrite(SENSORPOWERPIN,HIGH);    

    DeviceBase* dev;

    if(configuration["devicetype"] == "DHT22")
    {
      dev = new SIMPLEESPNOWCONNECTION_DHT22(configuration["devicetype"].as<String>());
    }

    dev->setDeviceParameter(configuration["device"]);

    SensorData* sd = dev->readParameterTypes();

    if(sd != NULL)
    {
      Serial.println(sd->size);

      for(int i = 0; i<sd->size; i++)
        Serial.println(sd->di[i].nameI);
    }

    delete sd;
    delete dev;
  }

  clockmillis = millis();
}

void setDeviceType(String deviceType)
{
#ifdef DEBUG
  Serial.println("Will set device type : "+deviceType);
#endif

  if(deviceType == "DHT22" ||
     deviceType == "BH1750" ||
     deviceType == "DS18B20" ||
     deviceType == "HTU21D" ||
     deviceType == "WATERSENSOR")
  {
    configuration["devicetype"] = deviceType;

    if(configuration.containsKey("device"))
    {
      configuration.remove("device");
    }

    device_configuration = configuration.createNestedObject("device");

    device_configuration["type"] = deviceType;

    if(deviceType == "DHT22")
    {
      device_configuration["pin"] = String(DHT22_PIN);
    }
      
    writeConfig();    
  }  
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
  int sleepSecs = configuration["interval"]; 
#ifdef DEBUG
  Serial.printf("Up for %i ms, going to sleep for %i secs...\n", millis(), sleepSecs); 
#endif

  ESP.deepSleep(sleepSecs * 1000000, RF_NO_CAL);
  delay(100);
}
