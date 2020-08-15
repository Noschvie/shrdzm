/*
  SHRDZMDevice

  Created 20 Jul 2020
  By Erich O. Pintar
  Modified 09 August 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#define DEBUG_SHRDZM

#include "config/config.h"

Configuration configuration;
SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);
DeviceBase* dev;
String ver, nam;
bool firmwareUpdate = false;
bool avoidSleeping = false;
bool batterycheckDone = false;
bool canGoDown = false;
bool loopDone = false;
bool initReboot = false;
bool sendBufferFilled = false;
bool isDeviceInitialized = false;
bool pairingOngoing = false;
bool finishSent = false;
String SSID;
String password;
String host;
String url;
unsigned long clockmillis = 0;
unsigned long prepareend = 0;
bool finalMeasurementDone = false;
bool setNewDeviceType = false;
String newDeviceType = "";


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

void sendSetup()
{
  simpleEspConnection.sendMessage((char *)"$I$");
  
  configuration.sendSetup(&simpleEspConnection);

  if(dev != NULL)
  {
    SensorData* sd = dev->readParameterTypes();

    if(sd != NULL)
    {
      String reply = "";
      
      reply = "$SD$";

      for(int i = 0; i<sd->size; i++)
      {
        reply += sd->di[i].nameI;
        if(i < sd->size-1)
          reply += "|";
      }

      simpleEspConnection.sendMessage((char *)reply.c_str());

      delete sd; 

      JsonObject ap = dev->getActionParameter();
      if(!ap.isNull())
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
  }

  String s = String("$V$")+ver+"-"+ESP.getSketchMD5();
  simpleEspConnection.sendMessage((char *)s.c_str());

  // send supported devices
  s = String("$X$")+String(SUPPORTED_DEVICES);
  simpleEspConnection.sendMessage((char *)s.c_str());  
}

bool updateFirmware(String message)
{
  if(message.indexOf('|') == -1)
  {
    DLN("firmware update not possible ! "+message);
    return false;
  }

  SSID = getValue(message, '|', 0);
  password = getValue(message, '|', 1);
  host = getValue(message, '|', 2);

  DLN("SSID:"+SSID+" password:"+password+" host:"+host);

  if(host.substring(0,7) != "http://")
  {
    DLN("Upgrade : only http address supported!");
    return false;    
  }

  host = host.substring(7);

  if(host.substring(host.length()-4) != ".php")
  {
    DLN("Upgrade : only php update script supported");
    return false;    
  }

  if(host.indexOf('/') == -1)
  {
    DLN("Upgrade : host string not valid");
    return false;    
  }

  url = host.substring(host.indexOf('/'));
  host = host.substring(0,host.indexOf('/'));

  DLN("before simpleEspConnection.end");
//  simpleEspConnection.end();
//  delay(100);
  DLN("after simpleEspConnection.end");
  WiFi.disconnect(true);
  
  WiFi.mode(WIFI_STA);
  DLN("after WIFI_STA ");

  WiFi.begin(SSID.c_str(), password.c_str());
  DLN("after Wifi.begin");

  return true;
}

void setConfig(String cmd)
{
  if(cmd == "configuration")
  {
    DLN("need to send the setup...");
    sendSetup();     

    return;
  }
  
  if(cmd.indexOf(':') == -1)
    return;

  String pname = getValue(cmd, ':', 0);
  String pvalue = getValue(cmd, ':', 1);

  DLN("setConfig "+pvalue);

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
      newDeviceType = pvalue;
      setNewDeviceType = true;
//      initDeviceType(pvalue.c_str(), true);
    }
    else
    {
      configuration.set((char *)pname.c_str(), (char *)pvalue.c_str());
    }
  }
  else
  {
    if(configuration.containsDeviceKey((char *)pname.c_str()))
    {
      configuration.setDeviceParameter((char *)pname.c_str(), (char *)pvalue.c_str());
    }
  }
       
  configuration.store();    
  sendSetup();       
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  DLN("MESSAGE:"+String((char *)message));

  if(String((char *)message) == "$SLEEP$") // force to go sleep
  {
    canGoDown = true;
    DLN("FORCE SLEEP MODE");
    return;
  }
  
  if(String((char *)message) == "$S$") // ask for settings
  {
    sendSetup();
  }
  else if(String((char *)message) == "$PING$") // ping
  {
    simpleEspConnection.sendMessage("$PING$");
      
    return;
  }
  else if(String((char *)message).substring(0,5) == "$SDT$") // set device type
  {
//    initDeviceType(String((char *)message).substring(5).c_str(), true);
    newDeviceType = String((char *)message).substring(5);
    setNewDeviceType = true;
    
    configuration.store();        
    sendSetup();
  }
  else if(String((char *)message).substring(0,4) == "$SC$") // set configuration
  {
    if(dev != NULL)
    {
      JsonObject ap = dev->getActionParameter();
      if(!ap.isNull())
      {
        String pname = getValue(String((char *)message).substring(4), ':', 0);      
        if(ap.containsKey(pname))
        {
          dev->setAction(String((char *)message).substring(4));
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
}

void OnPairingFinished()
{
  clockmillis = millis();  

  DLN("OnPairingFinished");

  sendSetup();
  avoidSleeping = false;
  pairingOngoing = false;

  clockmillis = millis();  

  initReboot = true;
}

void OnNewGatewayAddress(uint8_t *ga, String ad)
{
  simpleEspConnection.setServerMac(ga);
  configuration.set("gateway", (char *)ad.c_str());  

  configuration.store();   
}

void OnSendError(uint8_t* ad)
{
  
}

void OnSendDone(uint8_t* ad)
{
 // avoidSleeping = false;
}

// for firmware upgrade
void update_started() 
{
  DLN("CALLBACK:  HTTP update process started");
}

void update_finished() 
{
  DLN("CALLBACK:  HTTP update process finished ");
}

void update_progress(int cur, int total) 
{
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes..\n", cur, total);
}

void update_error(int err) 
{
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void upgradeFirmware()
{
  avoidSleeping = true;
  
  if ((WiFi.status() == WL_CONNECTED)) 
  {     
  // shrdzm.pintarweb.net fingerprint : 9A:10:D8:D2:DF:8B:C1:7C:36:B7:60:A2:F6:44:13:12:44:8B:91:FC 
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
  
    String versionStr = nam+" "+ver+" "+ESP.getSketchMD5();
    DLN("WLAN connected!");
  
    // char* fingerprint = "9A 10 D8 D2 DF 8B C1 7C 36 B7 60 A2 F6 44 13 12 44 8B 91 FC";
  
    WiFiClient client; 
    Serial.printf("host:%s, url:%s, versionString:%s \n", host.c_str(), url.c_str(), versionStr.c_str());
    t_httpUpdate_return ret = ESPhttpUpdate.update(host, 80, url, versionStr);    
//    t_httpUpdate_return ret = ESPhttpUpdate.update(client, host, 80, url, versionStr);    

  //      t_httpUpdate_return ret = ESPhttpUpdate.update(host, 443, url, versionStr, "9A:10:D8:D2:DF:8B:C1:7C:36:B7:60:A2:F6:44:13:12:44:8B:91:FC");    
    
    switch (ret) 
    {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d):  sendUpdatedVersion %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        delay(100);
        ESP.restart();
        break;
  
      case HTTP_UPDATE_NO_UPDATES:
        DLN("HTTP_UPDATE_NO_UPDATES ");
        delay(100);
        ESP.restart();
        break;
  
      case HTTP_UPDATE_OK:
        DLN("HTTP_UPDATE_OK");
        break;
    }
  }  
}

void initDeviceType(const char *deviceType, bool firstInit)
{
  delete dev;

  DLN("initDeviceType");

  if(strcmp(deviceType, "DHT22") == 0)
  {
    dev = new Device_DHT22();
  }
  else if(strcmp(deviceType, "BH1750") == 0)
  {
    dev = new Device_BH1750();
  }
  else if(strcmp(deviceType, "BMP280") == 0)
  {
    dev = new Device_BMP280();
  }
  else if(strcmp(deviceType, "BME280") == 0)
  {
    dev = new Device_BME280();
  }
  else if(strcmp(deviceType, "DS18B20") == 0)
  {
    dev = new Device_DS18B20();
  }
  else if(strcmp(deviceType, "HTU21D") == 0 || strcmp(deviceType, "HTU21") == 0 ||
          strcmp(deviceType, "SI7021") == 0 || strcmp(deviceType, "SHT21") == 0)
  {
    dev = new Device_HTU21D();
  }
  else if(strcmp(deviceType, "MQ135") == 0)
  {
    dev = new Device_MQ135();
  }
  else if(strcmp(deviceType, "WATER") == 0)
  {
    dev = new Device_WATER();
  }
  else if(strcmp(deviceType, "ANALOG") == 0)
  {
    dev = new Device_ANALOG();
  }
  else if(strcmp(deviceType, "DIGITAL") == 0)
  {
    dev = new Device_DIGITAL();
  }
  else if(strcmp(deviceType, "SDS011") == 0)
  {
    dev = new Device_SDS011();
  }
  else if(strcmp(deviceType, "DIGITALGROUND") == 0)
  {
    dev = new Device_DIGITALGROUND();
  }
  else if(strcmp(deviceType, "RELAYTIMER") == 0)
  {
    dev = new Device_RELAYTIMER();
  }
  else if(strcmp(deviceType, "GW60") == 0)
  {
    dev = new Device_GW60();
  }
  else
  {
    return;
  }
  
  if(dev != NULL)
  {
    configuration.set("devicetype", (char *)deviceType);
    
    if(firstInit)
    {     
      dev->initialize();
      
      JsonObject dc = dev->getDeviceParameter();

      configuration.setDeviceParameter(dc);

      SensorData *initParam = dev->readInitialSetupParameter();
  
      if(initParam)
      {
        for(int i = 0; i<initParam->size; i++)
        {
          if(configuration.containsKey((char *)initParam->di[i].nameI.c_str()))
          {
            configuration.set((char *)initParam->di[i].nameI.c_str(), (char *)initParam->di[i].valueI.c_str());
          }
        }
        
        delete initParam;
      }      

      initReboot = true;
    }

    dev->setDeviceParameter(configuration.getDeviceParameter());    
  }
}

void setup() 
{
#ifdef DEBUG_SHRDZM
Serial.begin(9600); Serial.println();
#endif
  bool writeConfigAndReboot = false;
  
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

  DV(nam);

  if(!SPIFFS.begin())
  {
    DLN("First use. I need to format file system. This will take a few seconds. Please wait...");
    SPIFFS.format();
  }

  if(!configuration.load())
  {
    configuration.initialize();
  }
  
  if(!configuration.containsKey("preparetime"))
  {
    configuration.set("preparetime", "0");
    writeConfigAndReboot = true;
  }

  if(!configuration.containsKey("processtime"))
  {
    configuration.set("processtime", "0");
    writeConfigAndReboot = true;
  }

  if(!configuration.containsKey("batterycheck"))
  {
    configuration.set("batterycheck", "OFF");
    writeConfigAndReboot = true;
  }  


  if(writeConfigAndReboot)
  {
    configuration.store();    

    delay(100);    
    ESP.restart();      
  }

  String lastVersionNumber = configuration.readLastVersionNumber();
  String currVersion = ESP.getSketchMD5();

  DLN("'"+lastVersionNumber+"':'"+currVersion+"'");
    
  pinMode(atoi(configuration.get("pairingpin")), INPUT_PULLUP);

  simpleEspConnection.begin();

  DV(simpleEspConnection.myAddress);
  
  simpleEspConnection.onPairingFinished(&OnPairingFinished);  
  simpleEspConnection.setPairingBlinkPort(LEDPIN);  
  simpleEspConnection.onSendError(&OnSendError);    
  simpleEspConnection.onSendDone(&OnSendDone);
  if(configuration.containsKey("gateway"))
  {
    simpleEspConnection.setServerMac(configuration.get("gateway"));  
  }

  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onMessage(&OnMessage);  

  if(digitalRead(atoi(configuration.get("pairingpin"))) == false)
  {
    avoidSleeping = true;
    DLN("Start pairing");    
    pairingOngoing = true;
    simpleEspConnection.startPairing(300);
  }
  else
  {
    // enable sensor power if configured
    if(atoi(configuration.get("sensorpowerpin")) != 99)
    {
      pinMode(atoi(configuration.get("sensorpowerpin")), OUTPUT);
      digitalWrite(atoi(configuration.get("sensorpowerpin")),HIGH);          
    }
    
    // check if preparation is needed
    prepareend = 1000 * atoi(configuration.get("preparetime"));

    if(strcmp(lastVersionNumber.c_str(), currVersion.c_str()) != 0)
    {    
      sendSetup();
      configuration.storeVersionNumber();
    }      
  }

  clockmillis = millis();  
}

void getMeasurementData()
{
  if(configuration.containsKey("gateway"))
  {      
    if(dev != NULL)
    {
      SensorData* sd = dev->readParameter();
  
      if(sd != NULL)
      {
        String reply;
        
        for(int i = 0; i<sd->size; i++)
        {
          reply = "$D$";
    
          reply += sd->di[i].nameI+":"+sd->di[i].valueI;

          DV(reply);
    
          simpleEspConnection.sendMessage((char *)reply.c_str());  
        }
        delete sd;
        sd = NULL;
      }
    }
  }  

//  postWait = millis() + 100;
}

void loop() 
{
  if(!firmwareUpdate)
    sendBufferFilled = simpleEspConnection.loop();

  if(pairingOngoing)
    return;

  if(firmwareUpdate)
    upgradeFirmware();

  if(!batterycheckDone)
  {
    batterycheckDone = configuration.get("batterycheck") == "ON" ? false : true;
    if(!batterycheckDone)
    {      
      String reply = "$D$battery:"+String(analogRead(A0));

      simpleEspConnection.sendMessage((char *)reply.c_str());  
      batterycheckDone = true;
    }
  }

  if(!isDeviceInitialized)
  {
    if(configuration.get("devicetype") != "UNKNOWN")
    {
      initDeviceType(configuration.get("devicetype"), false);
      if(dev != NULL)
        dev->prepare();
    }
    
    isDeviceInitialized = true;
  }

  
  // get measurement data
  if(dev != NULL)
  {
    loopDone = dev->loop();
    
    if(dev->isNewDataAvailable())
    {
      getMeasurementData();
    } 
  }  
  else
    loopDone = true;

  if(!finalMeasurementDone && millis() >= prepareend)
  {
    getMeasurementData();

    finalMeasurementDone = true;
    clockmillis = millis();
  }

  if(loopDone && !sendBufferFilled && !finishSent && finalMeasurementDone)
  {
    // send finish to gateway
    simpleEspConnection.sendMessage("$F$");  
    finishSent = true;
  }

  if(millis() > MAXCONTROLWAIT+clockmillis && !sendBufferFilled && loopDone & finalMeasurementDone && millis() >= prepareend)
  {
    canGoDown = true;
  }  

  if(setNewDeviceType && finalMeasurementDone)
  {
    initDeviceType(newDeviceType.c_str(), true);
    setNewDeviceType = false;
    newDeviceType = "";

    configuration.store();        
    DLN("vor sendSetup");
    sendSetup();    
    DLN("nach sendSetup");
  }


  if(canGoDown && !avoidSleeping && simpleEspConnection.isSendBufferEmpty())
  {
    if(initReboot)
    {
      ESP.restart();
      delay(100);

      return;
    }
    if(atoi(configuration.get("interval")) > 0)
      gotoSleep();    
  }
}

void gotoSleep() 
{  
  delete dev;  
  int sleepSecs;

  if(configuration.get("devicetype") == "UNKNOWN") // goto sleep just for 5 seconds and flash 2 times
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
    sleepSecs = atoi(configuration.get("interval")) - atoi(configuration.get("preparetime"));
  }

  Serial.printf("Up for %i ms, going to sleep for %i secs... \n", millis(), sleepSecs); 

  if(sleepSecs > 0)
  {
    ESP.deepSleep(sleepSecs * 1000000, RF_NO_CAL);
  }
  
  delay(100);
}
