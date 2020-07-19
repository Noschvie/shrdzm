/*
  SHRDZMSensor

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 25 June 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

//#define DISABLEGOTOSLEEP

#if defined(ESP8266)
#include <FS.H>
#else if defined(ESP32)
#include "SPIFFS.h"
#endif
#include <ArduinoJson.h>
#include "config/config.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>
#endif

#include "SimpleEspNowConnection.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);
DynamicJsonDocument configdoc(1024);
JsonObject configuration = configdoc.createNestedObject("configuration");
JsonObject device_configuration = configuration.createNestedObject("device");

String inputString;
String serverAddress;
unsigned long clockmillis = 0;
unsigned long prepareend = 0;
unsigned long processend = 0;
volatile bool pairingOngoing = false;
bool measurementDone = false;
DeviceBase* dev;
bool deviceTypeSet = true;
bool gatewayMessageDone = false;
long setupMessageHandled = 0;
int sensorDataSendCount = 0;
int configDataSendCount = 0;
String lastVersionNumber;
bool sendUpdatedVersion = false;
String ver, nam;
ConfigData *configData;
bool initRestart = false;
bool postActionDone = false;
bool processTimeActive = false;
bool loopDone = false;
bool actionSet = false;
bool batterycheckDone = false;
unsigned int postWait = 0;

int counter = 0;

#if defined(ESP8266)
ESP8266WiFiMulti WiFiMulti;
#endif
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
}

void update_progress(int cur, int total) 
{
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes..\n", cur, total);
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
#ifdef DEBUG
      Serial.println("reading config file");
#endif
      File configFile = SPIFFS.open("/shrdzm_config.json", "r");
      if (configFile) 
      {
#ifdef DEBUG
        Serial.println("opened config file");
#endif        
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
  String reply = "$SC$";

  simpleEspConnection.sendMessage((char *)"$I$");
  
  for (JsonPair kv : configuration) 
  {
    if(String(kv.key().c_str()) != "device")
      reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";
  }

  reply.remove(reply.length()-1);

  if(!configuration.containsKey("devicetype"))
  {
    reply += "|devicetype: ";
  }

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

  if(dev == NULL)
    actualizeDeviceType();

  if(dev != NULL)
  {
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

      delete sd; 
    }    
  }

  // Actionparameter
  if(dev != NULL)
  {
    JsonObject ap = dev->getActionParameter();
    if(ap != NULL)
    {
      reply = "$AP$";
  
      for (JsonPair kv : ap) 
      {
        reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";
      }
  
      reply.remove(reply.length()-1);
  
      simpleEspConnection.sendMessage((char *)reply.c_str());    
    }
  }
  
  String s = String("$V$")+ver+"-"+ESP.getSketchMD5();
  simpleEspConnection.sendMessage((char *)s.c_str());

  // send supported devices
  s = String("$X$")+String(SUPPORTED_DEVICES);
  simpleEspConnection.sendMessage((char *)s.c_str());
}

bool updateFirmware(String message)
{
#if defined(ESP8266)  
  if(message.indexOf('|') == -1)
  {
    Serial.println("firmware update not possible ! "+message);
    return false;
  }

  SSID = getValue(message, '|', 0);
  password = getValue(message, '|', 1);
  host = getValue(message, '|', 2);

  Serial.println("SSID:"+SSID+" password:"+password+" host:"+host);

  if(host.substring(0,7) != "http://")
  {
    Serial.println("Upgrade : only http address supported!");
    return false;    
  }

  host = host.substring(7);

  if(host.substring(host.length()-4) != ".php")
  {
    Serial.println("Upgrade : only php update script supported");
    return false;    
  }

  if(host.indexOf('/') == -1)
  {
    Serial.println("Upgrade : host string not valid");
    return false;    
  }

  url = host.substring(host.indexOf('/'));
  host = host.substring(0,host.indexOf('/'));
  
//  esp_now_deinit();
  simpleEspConnection.end();
//  WiFi.disconnect(true);
  delay(100);

  WiFi.mode(WIFI_STA);

//  WiFiMulti.addAP(SSID.c_str(), password.c_str());  
  WiFi.begin(SSID.c_str(), password.c_str());

  return true;
#else
  return false;
#endif  
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
#ifdef DEBUG
  Serial.println("MESSAGE:"+String((char *)message));
#endif

  if(String((char *)message) == "$SLEEP$") // force to go sleep
  {
    gatewayMessageDone = true;
#ifdef DEBUG
    Serial.println("FORCE SLEEP MODE");
#endif    
    return;
  }
  else if(String((char *)message) == "$PING$") // ping
  {
    simpleEspConnection.sendMessage("$PING$");
      
#ifdef DEBUG
    Serial.println("Pinged...");
#endif    
    return;
  }
  else if(String((char *)message).substring(0,3) == "$U$") // update firmware
  {
    firmwareUpdate = true;   

    if(!updateFirmware(String((char *)message).substring(3)))
    {
      delay(100);    
      ESP.restart();            
    }
  }
  else if(String((char *)message) == "$S$") // ask for settings
  {
    pairingOngoing = true;

    Serial.printf("ask for settings:devicetypeset %s\n", deviceTypeSet ? "YES" : "NO");

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
  else if(String((char *)message).substring(0,5) == "$SDT$") // set device type
  {
    pairingOngoing = true;

    setDeviceType(String((char *)message).substring(5));

    pairingOngoing = false;    
  }
  else if(String((char *)message).substring(0,4) == "$SC$") // set configuration
  {
    pairingOngoing = true;

    if(dev != NULL)
    {
      JsonObject ap = dev->getActionParameter();
      if(ap != NULL)
      {
        String pname = getValue(String((char *)message).substring(4), ':', 0);      
        if(ap.containsKey(pname))
        {
          dev->setAction(String((char *)message).substring(4));
          actionSet = true;
          measurementDone = getMeasurementData();
  
  //        processTimeActive = true;
        }
        else
        {
          setConfig(String((char *)message).substring(4));
        }
      }
      else
      {
        setConfig(String((char *)message).substring(4));
      }      
    }       
    else
    {
      setConfig(String((char *)message).substring(4));
    }

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
  simpleEspConnection.setServerMac(ga);
  configuration["gateway"] = ad;  

  writeConfig();    
}

void OnSendError(uint8_t* ad)
{
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
}

void OnSendDone(uint8_t* ad)
{
#ifdef DEBUG
//  Serial.println("Sending to '"+simpleEspConnection.macToStr(ad)+"' done");
#endif    
}

void actualizeDeviceType()
{
  delete dev;
  
  // setup the device
  if(configuration["devicetype"] == "DHT22")
  {
    dev = new Device_DHT22();
  }
  else if(configuration["devicetype"] == "BH1750")
  {
    dev = new Device_BH1750();
  }
  else if(configuration["devicetype"] == "BMP280")
  {
    dev = new Device_BMP280();
  }
  else if(configuration["devicetype"] == "BME280")
  {
    dev = new Device_BME280();
  }
  else if(configuration["devicetype"] == "DS18B20")
  {
    dev = new Device_DS18B20();
  }
  else if(configuration["devicetype"] == "HTU21D" || configuration["devicetype"] == "HTU21" ||
          configuration["devicetype"] == "SI7021" || configuration["devicetype"] == "SHT21")
  {
    dev = new Device_HTU21D();
  }
  else if(configuration["devicetype"] == "MQ135")
  {
    dev = new Device_MQ135();
  }
  else if(configuration["devicetype"] == "WATER")
  {
    dev = new Device_WATER();
  }
  else if(configuration["devicetype"] == "ANALOG")
  {
    dev = new Device_ANALOG();
  }
  else if(configuration["devicetype"] == "DIGITAL")
  {
    dev = new Device_DIGITAL();
  }
  else if(configuration["devicetype"] == "DIGITAL")
  {
    dev = new Device_DIGITALGROUND();
  }
  else if(configuration["devicetype"] == "SDS011")
  {
    dev = new Device_SDS011();
  }
  else if(configuration["devicetype"] == "RELAYTIMER")
  {
    dev = new Device_RELAYTIMER();
  }
  else if(configuration["devicetype"] == "GW60")
  {
    dev = new Device_GW60();
  }

  if(dev != NULL)
  {
    pairingOngoing = true;
    
    dev->setDeviceParameter(configuration["device"]);
  }
}

void setup() 
{
  Serial.begin(9600); Serial.println();
#ifdef DEBUG
  Serial.println(ESP.getSketchMD5());
#endif

#ifdef VERSION
  ver = String(VERSION);
#else
  ver = "0.0.0";  
#endif

#ifdef NAME
  nam = String(NAME);
#else
  nam = "SHRDZMDevice";  
#endif

  dev = NULL;
  bool writeConfigAndReboot = false;
  SPIFFS.begin();

  if(!readConfig())
  {
    SPIFFS.format();

    int i = SLEEP_SECS;
    configuration["interval"] = String(i);

    configuration["preparetime"] = "0";

    int s = SENSORPOWERPIN;
    configuration["sensorpowerpin"] = String(s);

    s = PAIRING_PIN;
    configuration["pairingpin"] = String(s);

    configuration["devicetype"] = "UNKNOWN";

    writeConfigAndReboot = true;
  }

  if(!configuration.containsKey("preparetime"))
  {
    configuration["preparetime"] = "0";
    writeConfigAndReboot = true;
  }

  if(!configuration.containsKey("processtime"))
  {
    configuration["processtime"] = "0";
    writeConfigAndReboot = true;
  }

  if(!configuration.containsKey("batterycheck"))
  {
    configuration["batterycheck"] = "OFF";
    writeConfigAndReboot = true;
  }
    

  if(writeConfigAndReboot)
  {
    writeConfig();    

    delay(100);    
    ESP.restart();      
  }

  readLastVersionNumber();
  String currVersion = ESP.getSketchMD5();

#ifdef DEBUG
  Serial.println( "'"+lastVersionNumber+"':'"+currVersion+"'");
#endif

  if(configuration["sensorpowerpin"].as<uint8_t>() != 99)
    pinMode(configuration["sensorpowerpin"].as<uint8_t>(), OUTPUT);
    
  pinMode(configuration["pairingpin"].as<uint8_t>(), INPUT_PULLUP);

  simpleEspConnection.begin();

#ifdef DEBUG
  Serial.println(simpleEspConnection.myAddress);
#endif
  
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
    Serial.println("Start pairing");    
    simpleEspConnection.startPairing(300);
  }
  else
  {
    // check if preparation is needed
    Serial.println( "vor preparetime");
    prepareend = 1000 * atoi(configuration["preparetime"]);
    Serial.println( "nach preparetime");
    
    if(configuration.containsKey("gateway"))
    {    
      if(configuration["sensorpowerpin"].as<uint8_t>() != 99)
      {
        pinMode(configuration["sensorpowerpin"].as<uint8_t>(),OUTPUT);        
        digitalWrite(configuration["sensorpowerpin"].as<uint8_t>(),HIGH);    
      }

      // setup the device
      if(configuration["devicetype"] == "DHT22")
      {
        dev = new Device_DHT22();
      }
      else if(configuration["devicetype"] == "BH1750")
      {
        dev = new Device_BH1750();
      }
      else if(configuration["devicetype"] == "BMP280")
      {
        dev = new Device_BMP280();
      }
      else if(configuration["devicetype"] == "BME280")
      {
        dev = new Device_BME280();
      }
      else if(configuration["devicetype"] == "DS18B20")
      {
        dev = new Device_DS18B20();
      }
      else if(configuration["devicetype"] == "HTU21D" || configuration["devicetype"] == "HTU21" ||
              configuration["devicetype"] == "SI7021" || configuration["devicetype"] == "SHT21")
      {
        dev = new Device_HTU21D();
      }
      else if(configuration["devicetype"] == "MQ135")
      {
        dev = new Device_MQ135();
      }
      else if(configuration["devicetype"] == "WATER")
      {
        dev = new Device_WATER();
      }
      else if(configuration["devicetype"] == "ANALOG")
      {
        dev = new Device_ANALOG();
      }
      else if(configuration["devicetype"] == "DIGITAL")
      {
        dev = new Device_DIGITAL();
      }
      else if(configuration["devicetype"] == "SDS011")
      {
        dev = new Device_SDS011();
      }
      else if(configuration["devicetype"] == "DIGITALGROUND")
      {
        dev = new Device_DIGITALGROUND();
      }
      else if(configuration["devicetype"] == "RELAYTIMER")
      {
        dev = new Device_RELAYTIMER();
      }
      else if(configuration["devicetype"] == "GW60")
      {
        dev = new Device_GW60();
      }

      if(dev != NULL)
      {
        dev->setDeviceParameter(configuration["device"]);
  
        if(strcmp(lastVersionNumber.c_str(), currVersion.c_str()) != 0)
        {    
          sendSetup();
          storeLastVersionNumber();
        }  

        dev->prepare();
      }
    } 
  }
  
  clockmillis = millis();
}

void setConfig(String cmd)
{
  if(cmd == "configuration")
  {
    sendSetup();     

    return;
  }
  
  if(cmd.indexOf(':') == -1)
    return;

  String pname = getValue(cmd, ':', 0);
  String pvalue = getValue(cmd, ':', 1);

#ifdef DEBUG
   Serial.println("setConfig "+pvalue);
#endif

  if( pname == "interval" || 
      pname == "sensorpowerpin" || 
      pname == "devicetype" || 
      pname == "preparetime" || 
      pname == "processtime" || 
      pname == "batterycheck" || 
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
     deviceType == "BMP280" ||
     deviceType == "BME280" ||
     deviceType == "DS18B20" ||
     deviceType == "HTU21D" ||
     deviceType == "HTU21" ||
     deviceType == "SI7021" ||
     deviceType == "SHT21" ||
     deviceType == "MQ135" ||
     deviceType == "ANALOG" ||
     deviceType == "DIGITAL" ||
     deviceType == "SDS011" ||
     deviceType == "DIGITALGROUND" ||
     deviceType == "RELAYTIMER" ||
     deviceType == "GW60" ||
     deviceType == "WATER")
  {
    configuration["devicetype"] = deviceType;

#ifdef DEBUG
   Serial.println("setDeviceType "+deviceType);
#endif

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
    else if(deviceType == "BMP280")
    {
      dev = new Device_BMP280();
    }
    else if(deviceType == "BME280")
    {
      dev = new Device_BME280();
    }
    else if(deviceType == "DS18B20")
    {
      dev = new Device_DS18B20();
    }
    else if(deviceType == "HTU21D" || deviceType == "HTU21" ||
            deviceType == "SI7021" || deviceType == "SHT21")
    {
      dev = new Device_HTU21D();
    }
    else if(deviceType == "MQ135")
    {
      dev = new Device_MQ135();
    }
    else if(deviceType == "WATER")
    {
      dev = new Device_WATER();
    }
    else if(deviceType == "ANALOG")
    {
      dev = new Device_ANALOG();
    }
    else if(deviceType == "DIGITAL")
    {
      dev = new Device_DIGITAL();
    }
    else if(deviceType == "SDS011")
    {
      dev = new Device_SDS011();
    }
    else if(deviceType == "DIGITALGROUND")
    {
      dev = new Device_DIGITALGROUND();
    }
    else if(deviceType == "RELAYTIMER")
    {
      dev = new Device_RELAYTIMER();
    }
    else if(deviceType == "GW60")
    {
      dev = new Device_GW60();
    }
    else
    {
      deviceTypeSet = false;
      return;
    }

    dev->initialize();
    
    JsonObject dc = dev->getDeviceParameter();
    configuration["device"] = dc;

    SensorData *initParam = dev->readInitialSetupParameter();

    if(initParam)
    {
      for(int i = 0; i<initParam->size; i++)
      {
        if(configuration.containsKey(initParam->di[i].nameI))
        {
          configuration[initParam->di[i].nameI] = initParam->di[i].valueI;
        }
      }
      
      delete initParam;
    }
      
    writeConfig();    
    
    sendSetup();
    delay(100);

    initRestart = true;
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
//  Serial.println("firmwareUpdate : "+String(firmwareUpdate));  
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

#if defined(ESP8266)
  int bytesWritten = file.write(v.c_str(), v.length());
#else if defined(ESP32)
  int bytesWritten = file.println(v);
#endif
   
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

bool getMeasurementData()
{
  if(configuration.containsKey("gateway"))
  {      
    if(dev != NULL)
    {
      pairingOngoing = true;
      
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
        delete sd;
        sd = NULL;
      }
      
      pairingOngoing = false;
    }
    else
    {
      Serial.println("Will not work until device type is set!");
    }
  }  

  measurementDone = true;
  postWait = millis() + 100;

  return true;
}

void loop() 
{
#if defined(ESP8266)
  if(firmwareUpdate)
  {    
//    if ((WiFiMulti.run() == WL_CONNECTED)) 
    if ((WiFi.status() == WL_CONNECTED)) 
    {     
     // firmwareUpdate = false;
      
  //    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);         
      // for firmware upgrade
	  // shrdzm.pintarweb.net fingerprint : 9A:10:D8:D2:DF:8B:C1:7C:36:B7:60:A2:F6:44:13:12:44:8B:91:FC 
      ESPhttpUpdate.onStart(update_started);
      ESPhttpUpdate.onEnd(update_finished);
      ESPhttpUpdate.onProgress(update_progress);
      ESPhttpUpdate.onError(update_error);

      String versionStr = nam+" "+ver+" "+ESP.getSketchMD5();
      Serial.println("WLAN connected!");

      char* fingerprint = "9A 10 D8 D2 DF 8B C1 7C 36 B7 60 A2 F6 44 13 12 44 8B 91 FC";

      WiFiClient client; 
  //    Serial.println("IP:"+ WiFi.localIP());
      Serial.printf("host:%s, url:%s, versionString:%s \n", host.c_str(), url.c_str(), versionStr.c_str());
      t_httpUpdate_return ret = ESPhttpUpdate.update(host, 80, url, versionStr);    
//      t_httpUpdate_return ret = ESPhttpUpdate.update(host, 443, url, versionStr, "9A:10:D8:D2:DF:8B:C1:7C:36:B7:60:A2:F6:44:13:12:44:8B:91:FC");    
      
      switch (ret) 
      {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d):  sendUpdatedVersion %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          delay(100);
          ESP.restart();
          break;
  
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES ");
          delay(100);
          ESP.restart();
          break;
  
        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      }
    }
  }
  else
  {
#endif

    if(pairingOngoing)
      return;
  
    if(!batterycheckDone)
    {
      batterycheckDone = configuration["batterycheck"] == "ON" ? false : true;
      if(!batterycheckDone)
      {      
        String reply = "$D$battery:"+String(analogRead(A0));
  
        simpleEspConnection.sendMessage((char *)reply.c_str());  
        batterycheckDone = true;
      }
    }
  
    if(!firmwareUpdate)
      simpleEspConnection.loop();
  
  //  if(dev != NULL && !loopDone && actionSet)
    if(dev != NULL)
    {
      if( !loopDone || atof(configuration["processtime"]) == 0.0f)
      {
        loopDone = dev->loop();
        if(dev->isNewDataAvailable())
        {
          getMeasurementData();
        }
      }
    }
  
    if(dev == NULL)
    {
      loopDone = true;
    }
  
    if(!measurementDone && millis() >= prepareend)
    {
      measurementDone = getMeasurementData();
  
      if(atof(configuration["processtime"]) > 0.0f)
      {
        processend = 1000 * atof(configuration["processtime"]) + millis();      
        postActionDone = false;    
        processTimeActive = true;     
      }
      else
      {
        postActionDone = true;         
        processTimeActive = false;     
      }
    }
  
    if(processTimeActive && !postActionDone && measurementDone && millis() >= processend)
    {
#ifdef DEBUG
        Serial.printf("Did wait %f seconds. Now I will start the postaction\n", atof(configuration["processtime"]));          
#endif   
      dev->setPostAction();
      measurementDone = getMeasurementData();
  
      postActionDone = true; 
      processTimeActive = false;  
    }
    
    while (Serial.available()) 
    {
      char inChar = (char)Serial.read();
      if (inChar == '\n') 
      {
        Serial.println(inputString);
  
        if(inputString == "startpair")
        {
          simpleEspConnection.startPairing(60);
        }
        else if(inputString.substring(0,14) == "setdevicetype ")
        {
          setDeviceType(inputString.substring(14));
          
          delay(100);
          ESP.restart();
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
  
    if(!measurementDone)
      return;
  
    if(!postActionDone && processTimeActive)
      return;
  
    if(processTimeActive)
      return;
  
    if(actionSet && !loopDone)  
      return;
  
    if(initRestart && simpleEspConnection.isSendBufferEmpty())
      ESP.restart();      
  
#ifndef DISABLEGOTOSLEEP    
    if(gatewayMessageDone || millis() > MAXCONTROLWAIT+clockmillis)  
    {
      if(pairingOngoing || !simpleEspConnection.isSendBufferEmpty() || firmwareUpdate)
        return;
  
      if(postWait > millis())
        return;
      
      // everything is done and I can go to sleep
      if(atoi(configuration["interval"]) > 0)
        gotoSleep();
    }
#endif   
  } 
}

void gotoSleep() 
{  
  delete dev;  
  int sleepSecs;

  if(configuration["devicetype"] == "UNKNOWN") // goto sleep just for 5 seconds and flash 2 times
  {
    sleepSecs = 5;
#ifdef LEDPIN
    pinMode(LEDPIN, OUTPUT);

    digitalWrite(LEDPIN, LOW);
    delay(500);
    digitalWrite(LEDPIN, HIGH);
    delay(500);
    digitalWrite(LEDPIN, LOW);
    delay(500);
    digitalWrite(LEDPIN, HIGH);
#endif
  }
  else
  {
    sleepSecs = atoi(configuration["interval"]) - atoi(configuration["preparetime"]);
  }

//#ifdef DEBUG
  Serial.printf("Up for %i ms, going to sleep for %i secs... \n", millis(), sleepSecs); 
//#endif

  if(sleepSecs > 0)
  {
#if defined (ESP8266)
    ESP.deepSleep(sleepSecs * 1000000, RF_NO_CAL);
#else if defined(ESP32)
#endif
  }
  else
  {
    measurementDone = false;
  }
  
  delay(100);
}
