/*
  SHRDZMDevice

  Created 20 Jul 2020
  By Erich O. Pintar
  Modified 26 December 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#define DEBUG_SHRDZM

#include "config/config.h"

Configuration configuration;
SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);
DeviceBase* dev;
DeviceBase* settingDev = NULL;
String ver, nam;
bool firmwareUpdate = false;
bool avoidSleeping = false;
bool batterycheckDone = false;
bool canGoDown = false;
bool forceSleep = false;
bool loopDone = false;
bool initReboot = false;
bool sendBufferFilled = false;
bool isDeviceInitialized = false;
bool pairingOngoing = false;
bool finishSent = false;
bool processendSet = false;
bool processendReached = false;
bool configurationMode = false;
bool gatewayMode = false;
bool firstMeasurement = true;
String SSID;
String password;
String host;
String url;
unsigned long clockmillis = 0;
unsigned long prepareend = 0;
unsigned long processend = 0;
unsigned long lastIntervalTime = 0;
unsigned long preparestart = 0;
unsigned long currentUptime = 0;
unsigned long configurationAPWaitStartTime = 0;
unsigned long configurationAPWaitOngoingStartTime = 0;
bool finalMeasurementDone = false;
bool setNewDeviceType = false;
bool preparing = false;
bool configurationAPWaiting = false;
bool configurationAPWaitOngoing = false;
String newDeviceType = "";
String deviceName;
bool sleepEnabled = true;
String lastRebootInfo = "";
bool apConnectingOngoing = false;
unsigned long apConnectionStartTime = 0;
unsigned long mqttNextTry = 0;
bool writeConfiguration = false;
Ticker configurationBlinker;
ESP8266WebServer server;
WiFiClient espClient;
PubSubClient mqttclient(espClient);
String MQTT_TOPIC = "SHRDZM/";
String subcribeTopicSet;
String subscribeTopicConfig;
char websideBuffer[6500];
char menuContextBuffer[4300];
char contentBuffer[750];
const uint16_t ajaxIntervall = 2;
String lastMessage = "";
bool cloudConnected = false;
String cloudToken = "";
String cloudID = "";
bool cloudIsDeviceRegistered = false;
time_t now;                         
tm tm; 
asyncHTTPrequest request;

typedef struct struct_esp_message {
  char prefix = '.';
  char type;
  long sendTime;
  uint8_t pc; // package count
  uint8_t p;  // package
  uint8_t len;
  char message[200];
  uint16_t checksum;
} esp_message;

/// Send ESP Message with checksum
void sendMessageWithChecksum(const char *message)
{
  uint8_t packages = strlen(message) / 200 + 1;
  long sendtime = millis();

  for(int i = 0; i<packages; i++)
  {
    esp_message em;
    
    em.type = 'F';
    em.sendTime = sendtime;
    em.pc = packages;
    em.p = i+1;
    em.len = strlen(message);    

    sprintf(em.message, message, strlen(message));
    em.checksum = simpleEspConnection.calculateChecksum(message);
  
    simpleEspConnection.sendMessage((uint8_t*)&em, sizeof(em));  
  }  
}

void controlConnectionBlinker(bool enabled)
{
#ifdef LEDPIN
    pinMode(LEDPIN, OUTPUT);  
    if(enabled)
    {
      configurationBlinker.attach(2.0, changeConfigurationBlinker);
    }
    else
    {
      configurationBlinker.detach(); 
      digitalWrite(LEDPIN, HIGH);
    }
#endif    
}

/// Configuration Webserver
void startConfigurationAP()
{
  configurationMode = true;
    
  String APName = "SHRDZMDevice-"+deviceName;
  WiFi.hostname(APName.c_str());        
  WiFi.softAP(APName);     

  DLN(F("Start configuration AP..."));
  
  server.on(F("/"), handleRoot);
  server.on(F("/reboot"), handleReboot);
  server.on(F("/general"), handleRoot);
  server.on(F("/settings"), handleSettings);
  server.on(F("/gateway"), handleGateway);
  server.on(F("/NTP"), handleNTP);
  server.on(F("/cloud"), handleCloud);
  server.onNotFound(handleNotFound);
  server.begin();


#ifdef LEDPIN
    pinMode(LEDPIN, OUTPUT);
#endif
  
  configurationBlinker.attach(0.2, changeConfigurationBlinker);
}

char* getWebsite(char* content, bool update = false)
{  
  int len = strlen(content);

  DLN("Content len = "+String(len));

  sprintf(websideBuffer,  
    website_template,
    update ? F("<script src=\"j.js\"></script>") : F(""), 
    deviceName.c_str(), deviceName.c_str(), content);

  return websideBuffer;
}

void handleJson() {
  // Output: send data to browser as JSON
  String message = "";

  time(&now);
  localtime_r(&now, &tm);

  char t[22];
  sprintf(t, "%4d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );

  message += (F("{\"mqttconnectionstate\":"));
  if(strcmp(configuration.getWlanParameter("MQTTenabled"), "true") == 0)
    message += (mqttclient.connected() ? F("\"Connected\"") : F("\"Not Connected\""));  
  else
    message += F("\"Disabled\"");    
  message += (F(",\"lastmessage\":"));
  message += lastMessage;  
  message += (F(",\"timestamp\":\""));
  message += String(t);  
  message += (F("\"}")); // End of JSON
  server.send(200, F("application/json"), message);
}

void handleJs()
{
  String message;
  message += F("const url ='json';\n"
               "function renew(){\n"
               " fetch(url)\n" // Call the fetch function passing the url of the API as a parameter
               " .then(response => {return response.json();})\n"
               " .then(jo => {\n"
               "   for (var i in jo)\n"
               "    {if (document.getElementById(i))\n"
               "      {\n"
               "          document.getElementById(i).innerHTML = jo[i];}\n"              
               "      }\n"
               " })\n"
               " .catch(function() {\n"                                        // this is where you run code if the server returns any errors
               " });\n"
               "}\n"
               "document.addEventListener('DOMContentLoaded', renew, setInterval(renew, ");
  message += ajaxIntervall * 1000;
  message += F("));");

  server.send(200, F("text/javascript"), message);
}

void handleRoot() 
{
  memset(contentBuffer, 0, sizeof(contentBuffer));

  if(server.hasArg(F("factoryreset")))
  {
    if(String(server.arg(F("factoryreset"))) == F("true")) // factory reset was pressed
    {
      snprintf(contentBuffer, 300,
       factoryreset_template      
      );
        
      server.send(200, F("text/html"), contentBuffer);

      configuration.resetConfiguration();

      delay(2000);
      
      ESP.restart();        
    }
  }

  if(server.hasArg(F("upgrade")) && server.hasArg(F("upgradepath")))
  {
    if(String(server.arg(F("upgrade"))) == F("true")) // Upgrade was pressed
    {
      DLN("Upgrade from "+String(server.arg(F("upgradepath")))+" will be started...");
      if(updateFirmwareByMQTT(server.arg(F("upgradepath"))))
      {        
        firmwareUpdate = true;

        snprintf(contentBuffer, 300, upgrade_template);
          
        server.send(200, F("text/html"), contentBuffer);  
        return;      
      }
    }    
  }
  
  sprintf(contentBuffer,  
      informationtable_template,
      ver.c_str(), ESP.getSketchMD5().c_str(),
      configuration.get("devicetype"),
      ESP.getChipId(),
      configuration.getWlanParameter("enabled"),
      configuration.get("gateway"),
      configuration.get("gateway"),deviceName.c_str(),
      configuration.get("gateway"),deviceName.c_str(),
      WiFi.localIP().toString().c_str(),
      WiFi.dnsIP().toString().c_str(),
      WiFi.gatewayIP().toString().c_str(),
      WiFi.subnetMask().toString().c_str()
  );  

  Serial.println(strlen(contentBuffer));
  
  sprintf(menuContextBuffer,  
      handleRoot_template,
      contentBuffer,
      gatewayMode ? upgradetext_template : ""
  );  

  char * temp = getWebsite(menuContextBuffer, true);
  server.send(200, F("text/html"), temp);
}

void handleNotFound() 
{
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, F("text/plain"), message);
}

void handleReboot() 
{
  char temp[300];
  
  snprintf(temp, 300, handleReboot_template);

  server.send(200, F("text/html"), temp);

  delay(2000);
  
  ESP.restart();  
}

void handleSettings()
{
  String deviceType;
  int loop = 0;
  JsonObject deviceParameter;
  SensorData* initialSettings = NULL;
  char parameterBuffer[2024];
  
  DV(sizeof(parameterBuffer));

  memset(contentBuffer, 0, sizeof(contentBuffer));
  memset(parameterBuffer, 0, sizeof(parameterBuffer));

  if(settingDev != NULL)
  {
    free(settingDev);
    settingDev = NULL;
  }

  if(server.hasArg(F("devicechanged")))
  {
    if(String(server.arg(F("devicechanged"))) == F("true")) // now to save the parameter to the configuration
    {
      configuration.removeAllDeviceParameter();
    }
  }  
  
  // check selected device
  if(server.hasArg(F("devices")))
    deviceType = server.arg(F("devices"));
  else
    deviceType = configuration.get("devicetype");       

  if(deviceType != "" && deviceType != F("UNKNOWN") && settingDev == NULL)
  {
    settingDev = createDeviceObject(deviceType.c_str());  
    settingDev->setConfigurationObject(configuration.getConfigDocument());
    settingDev->initialize();
  }
  else
  {
    configuration.removeAllDeviceParameter();
    configuration.set("devicetype", (char *)deviceType.c_str()); 

    writeConfiguration = true;          
    
    //configuration.store();    
  }
    
  // check if save was pressed
  if(server.hasArg(F("save")))
  {
    if(String(server.arg(F("save"))) == F("true")) // now to save the parameter to the configuration
    {
      configuration.set("devicetype", (char *)deviceType.c_str()); 
      
      JsonObject documentRoot = configuration.getConfigDocument()->as<JsonObject>();   
      for (JsonPair kv : documentRoot) 
      {
        if(server.hasArg(kv.key().c_str()))
        {
          configuration.set((char *)kv.key().c_str(), (char *)server.arg(kv.key().c_str()).c_str());
        }
      }   

      // get names for device parameter
      deviceParameter = settingDev->getDeviceParameter(); 
      if(!deviceParameter.isNull())
      {        
        for (JsonPair kv : deviceParameter)
        {
          if(server.hasArg(kv.key().c_str()))
          {
            configuration.setDeviceParameter(kv.key().c_str(), server.arg(kv.key().c_str()).c_str());
          }
          else
          {
            configuration.setDeviceParameter(kv.key().c_str(), kv.value().as<char*>());
          }
        }    
      }  

      writeConfiguration = true;          

//      configuration.store();
    }
  }
  
  if(settingDev != NULL)
  {
    if(strcmp(configuration.get("devicetype"), deviceType.c_str()) == 0)    
    {
      deviceParameter = configuration.getDeviceParameter();
    }
    else
    {
      deviceParameter = settingDev->getDeviceParameter();
      initialSettings = settingDev->readInitialSetupParameter();           
    }

    // Show general parameter
    JsonObject documentRoot = configuration.getConfigDocument()->as<JsonObject>();   
    for (JsonPair kv : documentRoot) 
    {
      if(String(kv.key().c_str()) != F("device") && String(kv.key().c_str()) != F("wlan") && String(kv.key().c_str()) != F("devicetype") && String(kv.key().c_str()) != F("cloud"))
      {
        strcat_P(parameterBuffer, PSTR("<br/><br/><div><label for='"));
        strcat(parameterBuffer, kv.key().c_str());
        strcat_P(parameterBuffer, PSTR("'>"));
        strcat(parameterBuffer, kv.key().c_str());
        strcat_P(parameterBuffer, PSTR("</label>"));
        strcat_P(parameterBuffer, PSTR("<input type='text' id='"));
        strcat(parameterBuffer, kv.key().c_str());
        strcat_P(parameterBuffer, PSTR("' name='"));
        strcat(parameterBuffer, kv.key().c_str());
        strcat_P(parameterBuffer, PSTR("' size='10' value='"));
        
        if(initialSettings != NULL && initialSettings->getDataItem(kv.key().c_str()) != "")
        {
          strcat(parameterBuffer, initialSettings->getDataItemPtr(kv.key().c_str()));
        }
        else        
        {
          strcat(parameterBuffer, kv.value().as<char*>());
        }
        
        strcat_P(parameterBuffer, PSTR("'></div>"));        
      }
    }    

    // Show device parameter
    if(!deviceParameter.isNull())
    {
      strcat_P(parameterBuffer, PSTR("<br/>"));        
      
      for (JsonPair kv1 : deviceParameter)
      {
        strcat_P(parameterBuffer, PSTR("<br/><br/><div><label for='"));        
        strcat(parameterBuffer, kv1.key().c_str());
        strcat_P(parameterBuffer, PSTR("'>"));        
        strcat(parameterBuffer, kv1.key().c_str());
        strcat_P(parameterBuffer, PSTR("</label>"));        
        
        strcat_P(parameterBuffer, PSTR("<input type='text' id='"));        
        strcat(parameterBuffer, kv1.key().c_str());
        strcat_P(parameterBuffer, PSTR("' name='"));        
        strcat(parameterBuffer, kv1.key().c_str());
        strcat_P(parameterBuffer, PSTR("' size='10' value='"));        
        strcat(parameterBuffer, kv1.value().as<char*>());
        strcat_P(parameterBuffer, PSTR("'></div>"));        
      }    
    }    
  } 
    
  // Fill select box
  strcpy_P(contentBuffer, PSTR("<option></option>"));

  char rest[strlen(SUPPORTED_DEVICES+1)];
  strcpy(rest, SUPPORTED_DEVICES);

  char *ptr = strtok(rest, ",");  // takes a list of delimiters
  
  while(ptr != NULL)
  {
    if(strcmp(ptr, deviceType.c_str()) == 0)
    {
        strcat_P(contentBuffer, PSTR("<option selected>"));
        strcat(contentBuffer, ptr);
        strcat_P(contentBuffer, PSTR("</option>"));
    }
    else
    {
        strcat_P(contentBuffer, PSTR("<option>"));
        strcat(contentBuffer, ptr);
        strcat_P(contentBuffer, PSTR("</option>"));
    }

    ptr = strtok(NULL, ",");  // takes a list of delimiters
  }

  DV(strlen(parameterBuffer));


  sprintf(menuContextBuffer,  
      settings_template,
      contentBuffer,
      parameterBuffer
  );  

  char * temp = getWebsite(menuContextBuffer);
  DV(temp);
  DLN("after getWebsite size = "+String(strlen(temp)));

  server.send(200, F("text/html"), temp);  
}

void handleCloud()
{
  bool gatewayipset = WiFi.localIP().toString() != F("(IP unset)");
  
  if(server.args() != 0)
  {
    if( server.hasArg(F("cloudEnabledChanged")))
    {
      DV(String(server.arg(F("cloudEnabledChanged"))));
      if(String(server.arg(F("cloudEnabledChanged"))) == F("1")) 
      {
        if( server.hasArg(F("cloudenabled")))
        {
          if(String(server.arg(F("cloudenabled"))) == F("on"))
          {
            if(server.arg(F("user")) != "" && server.arg(F("password")) != "")
            {
              configuration.setCloudParameter("enabled", "true");

              if(gatewayipset)
              {
                if(cloudRegisterNewUser(server.arg("user").c_str(), "", server.arg("password").c_str()))
                {
                  cloudConnected = cloudLogin(server.arg("user").c_str(), server.arg("password").c_str());
                  if(cloudConnected)
                  {
                    configuration.setCloudParameter("userid", cloudID.c_str());
                    configuration.setCloudParameter("user", server.arg(F("user")).c_str());
                    configuration.setCloudParameter("password", server.arg(F("password")).c_str());
                    cloudRegisterDevice(deviceName.c_str(), configuration.get("devicetype"));
                  }
                }
              }
            }
            else
            {
              // show error
            }
          }
          else
          {
            if(gatewayipset)
            {
              cloudUnregisterDevice(deviceName.c_str());
              cloudUnregisterUser(configuration.getCloudParameter("user"), configuration.getCloudParameter("password"));
            }
            configuration.setCloudParameter("enabled", "false");
            configuration.setCloudParameter("userid", "");
          }
        }
        else
        {
          cloudUnregisterDevice(deviceName.c_str());
          cloudUnregisterUser(configuration.getCloudParameter("user"), configuration.getCloudParameter("password"));
          configuration.setCloudParameter("enabled", "false");
          configuration.setCloudParameter("userid", "");
        }
      }      
    }
    if( server.hasArg(F("resetCloudSettings")))
    {
      if(String(server.arg(F("resetCloudSettings"))) == F("true"))
      {
        if(gatewayipset)
        {
          cloudUnregisterDevice(deviceName.c_str());
          cloudUnregisterUser(configuration.getCloudParameter("userid"), configuration.getCloudParameter("password"));
        }        
        configuration.setCloudParameter("enabled", "false");
        configuration.setCloudParameter("user", deviceName.c_str());
        configuration.setCloudParameter("password", String(ESP.getChipId()).c_str());
      }
    }
    
    writeConfiguration = true;          
  }  
  
  sprintf(menuContextBuffer, handleCloud_template,
      String(configuration.getCloudParameter("enabled")) == "true" ? "checked" : "",
      strcmp(configuration.getCloudParameter("user"), "") == 0 ? deviceName.c_str() :
        configuration.getCloudParameter("user"),
      strcmp(configuration.getCloudParameter("password"), "") == 0 ? String(ESP.getChipId()).c_str() :
        configuration.getCloudParameter("password"),
      (strlen(configuration.getCloudParameter("userid")) == 0) ? "<i><div style='color:#FF0000';>NOT REGISTERED</div></i>" : configuration.getCloudParameter("userid")
  );  

  char * temp = getWebsite(menuContextBuffer);

  server.send(200, F("text/html"), temp);    
}

void handleNTP()
{
  if(server.args() != 0)
  {
    if(server.hasArg(F("ntpserver")))
      configuration.setWlanParameter("NTPServer", server.arg(F("ntpserver")).c_str());

    if(server.hasArg(F("tz")))
      configuration.setWlanParameter("TZ", server.arg(F("tz")).c_str());
        
    writeConfiguration = true;          
  }
    
  sprintf(menuContextBuffer, handleNTP_template,
        configuration.getWlanParameter("NTPServer"),
        configuration.getWlanParameter("TZ")      
      );  

  char * temp = getWebsite(menuContextBuffer);

  DLN("after getWebsite size = "+String(strlen(temp)));
  server.send(200, F("text/html"), temp);  
}

void handleGateway()
{
  if(server.args() != 0)
  {
    if( server.arg(F("wlanenabled")) == "1")
    {
      configuration.setWlanParameter("enabled", "true");
      configuration.set("gateway", (char *)deviceName.c_str());
    }
    else
      configuration.setWlanParameter("enabled", "false");

    if( server.arg(F("mqttenabled")) == "1")
    {
      configuration.setWlanParameter("MQTTenabled", "true");
    }
    else
      configuration.setWlanParameter("MQTTenabled", "false");

    if( server.arg(F("jsonenabled")) == "1")
    {
      configuration.setWlanParameter("MQTTsendjson", "true");
    }
    else
      configuration.setWlanParameter("MQTTsendjson", "false");

    if(server.hasArg(F("ssid")))
      configuration.setWlanParameter("ssid", server.arg(F("ssid")).c_str());
    else
      configuration.setWlanParameter("ssid", "");
        
    if(server.hasArg(F("password")))
      configuration.setWlanParameter("password", server.arg(F("password")).c_str());
    else
      configuration.setWlanParameter("password", "");    

    if(server.hasArg(F("ip")))
      configuration.setWlanParameter("ip", server.arg(F("ip")).c_str());
    else
      configuration.setWlanParameter("ip", "");    
    
    if(server.hasArg(F("dns")))
      configuration.setWlanParameter("dns", server.arg(F("dns")).c_str());
    else
      configuration.setWlanParameter("dns", "");    
    
    if(server.hasArg(F("gateway")))
      configuration.setWlanParameter("gateway", server.arg(F("gateway")).c_str());
    else
      configuration.setWlanParameter("gateway", "");    
    
    if(server.hasArg(F("subnet")))
      configuration.setWlanParameter("subnet", server.arg(F("subnet")).c_str());
    else
      configuration.setWlanParameter("subnet", "");    
    
    if(server.hasArg("MQTTbroker"))
      configuration.setWlanParameter("MQTTbroker", server.arg(F("MQTTbroker")).c_str());
    if(server.hasArg(F("MQTTport")))
      configuration.setWlanParameter("MQTTport", server.arg(F("MQTTport")).c_str());
    if(server.hasArg(F("MQTTuser")))
      configuration.setWlanParameter("MQTTuser", server.arg(F("MQTTuser")).c_str());
    if(server.hasArg(F("MQTTpassword")))
      configuration.setWlanParameter("MQTTpassword", server.arg(F("MQTTpassword")).c_str());

    writeConfiguration = true;    
  }
   
  sprintf(menuContextBuffer, handleGateway_template,
      String(configuration.getWlanParameter("enabled")) == "true" ? "checked" : "",
      configuration.getWlanParameter("ssid"),
      configuration.getWlanParameter("password"),
      configuration.getWlanParameter("ip"),
      configuration.getWlanParameter("dns"),
      configuration.getWlanParameter("gateway"),
      configuration.getWlanParameter("subnet"),
      String(configuration.getWlanParameter("MQTTenabled")) == "false" ? "" : "checked",
      configuration.getWlanParameter("MQTTbroker"),
      configuration.getWlanParameter("MQTTport"),
      configuration.getWlanParameter("MQTTuser"),
      configuration.getWlanParameter("MQTTpassword"),
      String(configuration.getWlanParameter("MQTTsendjson")) == "false" ? "" : "checked"
  );  

  char * temp = getWebsite(menuContextBuffer);

  DLN("after getWebsite size = "+String(strlen(temp)));
  server.send(200, F("text/html"), temp);
}

///////////////////////////


/// Gateway Webserver
bool mqttreconnect() 
{
  // Loop until we're reconnected
  if (!mqttclient.connected()) 
  {
    DLN("Attempting MQTT connection...");
    String clientId = "SHRDZMDevice-"+deviceName;
    // Attempt to connect
    if(mqttclient.connect(deviceName.c_str(), configuration.getWlanParameter("MQTTuser"), configuration.getWlanParameter("MQTTpassword")))
    {
      subcribeTopicSet = String(MQTT_TOPIC)+"/set";
      subscribeTopicConfig = String(MQTT_TOPIC)+"/config/set";
      
      DLN("connected");
      
      DLN("MQTTHost : "+String(configuration.getWlanParameter("MQTTbroker")));
      DLN("MQTTPort : "+String(configuration.getWlanParameter("MQTTport")));
      DLN("MQTTUser : "+String(configuration.getWlanParameter("MQTTuser")));
      DLN("MQTTPassword : xxxxxxxxxxxxxxxxxxx");
      DLN("MQTT_TOPIC : "+MQTT_TOPIC);
      DLN("MQTT_TOPIC_SUBSCRIBE Set : "+String(subcribeTopicSet));
      DLN("MQTT_TOPIC_SUBSCRIBE Config : "+String(subscribeTopicConfig));
            
      // ... and resubscribe
      if(!mqttclient.subscribe(subcribeTopicSet.c_str()))
        DLN("Error at subscribe");
        
      mqttclient.subscribe(subscribeTopicConfig.c_str());

      // Once connected, publish an announcement...
      mqttclient.publish((String(MQTT_TOPIC)+"/state").c_str(), "up");
      mqttclient.publish((String(MQTT_TOPIC)+"/IP").c_str(), WiFi.localIP().toString().c_str()); 

      return true;
    } 
    else
      return false;
  }
  else
    return true;
    
  return false;
}

void mqttcallback(char* topic, byte* payload, unsigned int len) 
{
  char* p = (char*)malloc(len+1);
  memcpy(p,payload,len);
  p[len] = '\0';
  String cmd = String(p);
  free(p);  

  DLN("Message arrived [");
  DLN(topic);
  DLN("]: ");
  DLN(String((char *)payload));

  if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd == "reset")
  {
    mqttclient.publish((String(MQTT_TOPIC)+"/state").c_str(), "reset");
    delay(1);

    ESP.restart();
    delay(100);
  }  
  else if(String(topic) == subscribeTopicConfig)
  {
    StringSplitter *splitter = new StringSplitter(cmd, ' ', 4);
    int itemCount = splitter->getItemCount();

    if(itemCount == 2)
    {
      if(splitter->getItemAtIndex(0) == deviceName)
      {
        if(splitter->getItemAtIndex(1) == F("configuration"))
        {
          sendSetup();
        }
      }
    }
    else if(itemCount == 3)
    {
      if(splitter->getItemAtIndex(1) == F("upgrade"))
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/state").c_str(), "upgrade");   

        firmwareUpdate = true;   

        if(!updateFirmwareByMQTT(splitter->getItemAtIndex(2)))
        {
          delay(100);    
          ESP.restart();            
        }
      }
      else 
      {
        String message = splitter->getItemAtIndex(1)+":"+splitter->getItemAtIndex(2);

        if(dev != NULL)
        {
          JsonObject ap = dev->getActionParameter();
          if(!ap.isNull())
          {
            if(ap.containsKey(splitter->getItemAtIndex(1)))
            {
              dev->setAction(message);
            }
            else
            {
              setConfig(message);
            }
          }
          else
          {
            setConfig(message);
          }      
        }       
        else
        {
          setConfig(message);
        }               
      }
    }    
  }
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,5) == F("pair "))
  {
    mqttclient.publish((String(MQTT_TOPIC)+"/paired").c_str(), String(deviceName+"/"+deviceName).c_str());
    sendSetup();
  }
}

void startGatewayWebserver()
{
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_STA);
  DLN("after WIFI_STA ");
  IPAddress ip;
  IPAddress dns;
  IPAddress gateway;
  IPAddress subnet;

  String APName = "SHRDZMDevice-"+deviceName;
  MQTT_TOPIC = "SHRDZM/"+deviceName;
  
  WiFi.hostname(APName.c_str());

  StringSplitter *ipparts = new StringSplitter(configuration.getWlanParameter("ip"), '.', 4);
  if(ipparts->getItemCount() == 4)
  {
    ip.fromString(configuration.getWlanParameter("ip"));
    dns.fromString(configuration.getWlanParameter("dns"));
    gateway.fromString(configuration.getWlanParameter("gateway"));
    subnet.fromString(configuration.getWlanParameter("subnet"));
    
    WiFi.config(ip, dns, gateway, subnet);
  }
  WiFi.begin(configuration.getWlanParameter("ssid"), configuration.getWlanParameter("password"));
  DLN("after Wifi.begin");

  apConnectingOngoing = true;
  apConnectionStartTime = millis();

  controlConnectionBlinker(true);
                       
  mqttclient.setServer(configuration.getWlanParameter("MQTTbroker"), 
                       atoi(configuration.getWlanParameter("MQTTport")));

  mqttclient.setCallback(mqttcallback);

  if(configuration.containsWlanKey("NTPServer") && String(configuration.getWlanParameter("NTPServer")) != "" )
  {
    if(configuration.containsWlanKey("TZ") && String(configuration.getWlanParameter("TZ")) != "" )
    {
      configTime(configuration.getWlanParameter("TZ"), configuration.getWlanParameter("NTPServer"));
    }
  }

#ifdef DEBUG_SHRDZM
  request.setDebug(true);
#endif  
  request.onReadyStateChange(requestCB);  
}

///////////////////////////
// Create device object
///////////////////////////

DeviceBase * createDeviceObject(const char *deviceType)
{  
  if(strcmp(deviceType, "DHT22") == 0)
  {
    return new Device_DHT22();
  }
  else if(strcmp(deviceType, "BH1750") == 0)
  {
    return new Device_BH1750();
  }
  else if(strcmp(deviceType, "BMP280") == 0)
  {
    return new Device_BMP280();
  }
  else if(strcmp(deviceType, "BME280") == 0)
  {
    return new Device_BME280();
  }
  else if(strcmp(deviceType, "DS18B20") == 0)
  {
    return new Device_DS18B20();
  }
  else if(strcmp(deviceType, "HTU21D") == 0 || strcmp(deviceType, "HTU21") == 0 ||
          strcmp(deviceType, "SI7021") == 0 || strcmp(deviceType, "SHT21") == 0)
  {
    return new Device_HTU21D();
  }
  else if(strcmp(deviceType, "MQ135") == 0)
  {
    return new Device_MQ135();
  }
  else if(strcmp(deviceType, "WATER") == 0)
  {
    return new Device_WATER();
  }
  else if(strcmp(deviceType, "ANALOG") == 0)
  {
    return new Device_ANALOG();
  }
  else if(strcmp(deviceType, "DIGITAL") == 0)
  {
    return new Device_DIGITAL();
  }
  else if(strcmp(deviceType, "SDS011") == 0)
  {
    return new Device_SDS011();
  }
  else if(strcmp(deviceType, "IM350/AM550") == 0)
  {
    return new Device_IM350();
  }
  else if(strcmp(deviceType, "SDS011_BMP280") == 0)
  {
    return new Device_SDS011_BMP280();
  }  
  else if(strcmp(deviceType, "SDS011_BME280") == 0)
  {
    return new Device_SDS011_BME280();
  }  
  else if(strcmp(deviceType, "SDS011_BH1750") == 0)
  {
    return new Device_SDS011_BH1750();
  }  
  else if(strcmp(deviceType, "DIGITALGROUND") == 0)
  {
    return new Device_DIGITALGROUND();
  }
  else if(strcmp(deviceType, "RELAYTIMER") == 0)
  {
    return new Device_RELAYTIMER();
  }
  else if(strcmp(deviceType, "GW60") == 0)
  {
    return new Device_GW60();
  }
  else if(strcmp(deviceType, "DOORSENSOR") == 0)
  {
    return new Device_DOORSENSOR();
  }  
  else
  {
    DLN("Device Type "+String(deviceType)+" not known");
    return NULL;
  }

  return NULL;
}

///////////////////////////
String macToStr(const uint8_t* mac)
{
  char mac_addr[13];
  mac_addr[12] = 0;
  
  sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

  return String(mac_addr);
}

void changeConfigurationBlinker()
{
#ifdef LEDPIN
  digitalWrite(LEDPIN, !(digitalRead(LEDPIN)));
#endif
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

void sendSetup()
{ 
  if(!gatewayMode && !configuration.containsKey("gateway"))
    return;

  avoidSleeping = true;

  if(!gatewayMode)
  {
    DLN("Send INIT");
    sendMessageWithChecksum((char *)"$I$");
    configuration.sendSetup(&simpleEspConnection);
  }
  else
  {
    mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/init").c_str(), "INIT");
    configuration.sendSetup(&mqttclient, (String(MQTT_TOPIC)+"/"+deviceName).c_str());
  }

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

        if(gatewayMode)
          mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/param").c_str(), (sd->di[i].nameI).c_str());
      }

      if(!gatewayMode)
      {
      //  simpleEspConnection.sendMessage((char *)reply.c_str());
        sendMessageWithChecksum((char *)reply.c_str());
        DV(reply);
      }      
      
      delete sd; 

      JsonObject ap = dev->getActionParameter();
      if(!ap.isNull())
      {
        reply = "$AP$";
    
        for (JsonPair kv : ap) 
        {
          reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";

          if(gatewayMode)
            mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/actions").c_str(), (kv.key().c_str()+String(":")+kv.value().as<char*>()).c_str());
        }
    
        reply.remove(reply.length()-1);
    
        if(!gatewayMode)
        {
         // simpleEspConnection.sendMessage((char *)reply.c_str());    
         sendMessageWithChecksum((char *)reply.c_str());
        }
      }      
    }        
  }

  String s = String("$V$")+ver+"-"+ESP.getSketchMD5();

  if(!gatewayMode)
  {
   // simpleEspConnection.sendMessage((char *)s.c_str());
   sendMessageWithChecksum((char *)s.c_str());   
  }
  else
    mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/version").c_str(), (ver+"-"+ESP.getSketchMD5()).c_str());
  

  // send supported devices
  s = String("$X$")+String(SUPPORTED_DEVICES);
  if(!gatewayMode)
  {
    sendMessageWithChecksum((char *)s.c_str());   
  }
  else
    mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/sensors").c_str(), (String(SUPPORTED_DEVICES)).c_str());

  avoidSleeping = false;    
}

// Upgrade firmware via inbuild gateway
bool updateFirmwareByMQTT(String message)
{
  host = message;
  
  if(host.substring(0,7) != F("http://"))
  {
    DLN("Upgrade : only http address supported! ");
    return false;    
  }

  host = host.substring(7);

  if(host.substring(host.length()-4) != F(".php"))
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
  
  return true;
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

  if(host.substring(0,7) != F("http://"))
  {
    DLN("Upgrade : only http address supported!");
    return false;    
  }

  host = host.substring(7);

  if(host.substring(host.length()-4) != F(".php"))
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

  WiFi.disconnect(true);
  
  WiFi.mode(WIFI_STA);
  DLN("after WIFI_STA ");

  WiFi.begin(SSID.c_str(), password.c_str());
  DLN("after Wifi.begin");

  return true;
}

void setConfig(String cmd)
{
  if(cmd == F("configuration"))
  {
    DLN("need to send the setup...");
    sendSetup();     

    return;
  }
  
  if(cmd.indexOf(':') == -1)
    return;

  String pname = getValue(cmd, ':', 0);
  String pvalue = getValue(cmd, ':', 1);

  DLN("setConfig Name="+pname+", Value="+pvalue);

  if( pname == F("interval") || 
      pname == F("sensorpowerpin") || 
      pname == F("devicetype") || 
      pname == F("preparetime") || 
      pname == F("processtime") || 
      pname == F("batterycheck") || 
      pname == F("gateway"))
  {
    if(pname == F("devicetype"))
    {
      newDeviceType = pvalue;
      setNewDeviceType = true;
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

  if(String((char *)message) == F("$SLEEP$")) // force to go sleep
  {
    canGoDown = true;
    DLN("FORCE SLEEP MODE");
    forceSleep= true;
    return;
  }
  
  if(String((char *)message) == F("$S$")) // ask for settings
  {
    sendSetup();
  }
  else if(String((char *)message) == F("$PING$")) // ping
  {
    if(configuration.containsKey("gateway"))
    {
      // simpleEspConnection.sendMessage("$PING$");
      sendMessageWithChecksum("$PING$");        
    }
      
    return;
  }
  else if(String((char *)message).substring(0,5) == F("$SDT$")) // set device type
  {
    newDeviceType = String((char *)message).substring(5);
    setNewDeviceType = true;
    
    configuration.store();        
    sendSetup();
  }
  else if(String((char *)message).substring(0,4) == F("$SC$")) // set configuration
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
  else if(String((char *)message).substring(0,3) == F("$U$")) // update firmware
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

//  sendSetup();
  avoidSleeping = false;
  pairingOngoing = false;

  clockmillis = millis();  

  initReboot = true;
}

void OnNewGatewayAddress(uint8_t *ga, String ad)
{
  simpleEspConnection.setServerMac(ga);
  configuration.set("gateway", (char *)ad.c_str());  
  configuration.setWlanParameter("enabled", "false");  

  DV((char *)ad.c_str());

  pairingOngoing = false;
 // writeConfiguration = true;
  
  configuration.store();   

  sendSetup();  
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
  DLN("CALLBACK:  HTTP update process at "+String(cur)+" of "+String(total)+" bytes..");
}

void update_error(int err) 
{
  DLN("CALLBACK:  HTTP update fatal error code "+ String(err));
}

void upgradeFirmware()
{
  avoidSleeping = true;
  
  if ((WiFi.status() == WL_CONNECTED)) 
  {     
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
  
    String versionStr = nam+" "+ver+" "+ESP.getSketchMD5();
    DLN("WLAN connected! ");
  
    WiFiClient client; 
    // Serial.printf("host:%s, url:%s, versionString:%s \n", host.c_str(), url.c_str(), versionStr.c_str());
//    t_httpUpdate_return ret = ESPhttpUpdate.update(host, 80, url, versionStr);    
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, host, 80, url, versionStr);    
    
    switch (ret) 
    {
      case HTTP_UPDATE_FAILED:
     //   Serial.printf("HTTP_UPDATE_FAILD Error (%d): sendUpdatedVersion %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
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

void initDeviceType(const char *deviceType, bool firstInit, bool reboot=true)
{
  delete dev;

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
  else if(strcmp(deviceType, "IM350/AM550") == 0)
  {
    dev = new Device_IM350();
  }
  else if(strcmp(deviceType, "SDS011_BMP280") == 0)
  {
    dev = new Device_SDS011_BMP280();
  }  
  else if(strcmp(deviceType, "SDS011_BME280") == 0)
  {
    dev = new Device_SDS011_BME280();
  }  
  else if(strcmp(deviceType, "SDS011_BH1750") == 0)
  {
    dev = new Device_SDS011_BH1750();
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
  else if(strcmp(deviceType, "DOORSENSOR") == 0)
  {
    dev = new Device_DOORSENSOR();
  }  
  else
  {
    return;
  }
  
  if(dev != NULL)
  {
    configuration.set("devicetype", (char *)deviceType);
    dev->setConfigurationObject(configuration.getConfigDocument());
    
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

      if(reboot)
        initReboot = true;
    }

    dev->setDeviceParameter(configuration.getDeviceParameter());    

    Serial.println("nach setDeviceParameter");
    
  }
}

void setup() 
{
#ifdef DEBUG_SHRDZM
Serial.begin(SERIAL_BAUD); Serial.println();
#endif
  DV("setup start ");  
  
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

  WiFi.persistent(false);

  // set device name
  WiFi.mode(WIFI_STA);  
  uint8_t pmac[6];
  WiFi.macAddress(pmac);
  deviceName = macToStr(pmac);
  
  deviceName.replace(":", "");
  deviceName.toUpperCase();

  configuration.setDeviceName(deviceName.c_str());

  DV(deviceName);

#ifdef LITTLEFS
  if(!LittleFS.begin())
  {
    DLN("First use. I need to format file system. This will take a few seconds. Please wait...");
    LittleFS.format();
    LittleFS.begin();  
  }
  else
  {
    DLN("LittleFS accessed...");
  }
#else
  if(!SPIFFS.begin())
  {
    DLN("First use. I need to format file system. This will take a few seconds. Please wait...");
    SPIFFS.format();
    SPIFFS.begin();  
  }
  else
  {
   // DLN("SPIFFS accessed...");
  }
#endif
  
  if(!configuration.load())
  {
    DLN("configuration.initialize...");    
    configuration.initialize();
    
    configuration.store();    

    delay(100);    
    ESP.restart();      
  }

  if(!configuration.checkCompatibility())
  {    
#ifdef LITTLEFS
    LittleFS.format();
#else
    SPIFFS.format();
#endif    
//    configuration.initialize();
//    configuration.store();    

    delay(100);    
    ESP.restart();      
  }  

  if(configuration.migrateToNewConfigurationStyle())
  {
    DLN("Migration to new config style needed. Will reboot now...");    

    configuration.store();    

    delay(100);    
    ESP.restart();      
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

  if(configuration.get("pairingpin") != NULL)
  {    
    pinMode(atoi(configuration.get("pairingpin")), INPUT_PULLUP);    
  }
  else
  {
    int s = 13;
#ifdef PAIRING_PIN    
    s = PAIRING_PIN;
#endif
    configuration.set("pairingpin", (char *)(String(s).c_str()));
    configuration.store();    

    delay(100);    
    ESP.restart();      
  }
    
  // enable sensor power if configured
  if(atoi(configuration.get("sensorpowerpin")) != 99)
  {
    pinMode(atoi(configuration.get("sensorpowerpin")), OUTPUT);
    digitalWrite(atoi(configuration.get("sensorpowerpin")),HIGH);          
  }

  // check if pairing button pressed
  pairingOngoing = !digitalRead(atoi(configuration.get("pairingpin")));

  // check last boot info
  lastRebootInfo = configuration.readLastRebootInfo();

  configuration.storeLastRebootInfo("normal");
  
  if(lastRebootInfo == F("connectiontimeout") && !pairingOngoing)
  {
    startConfigurationAP();
    return;
  }

  /// check whether to start in gateway mode
  if( String(configuration.getWlanParameter("enabled")) == "true" && !pairingOngoing)
    gatewayMode = true;

  if(gatewayMode)
  {
    startGatewayWebserver();    
    mqttNextTry = 0;
    clockmillis = millis();      
    return;
  }  

  if((configuration.get("gateway") == NULL || configuration.get("gateway") == "") && !pairingOngoing)
  {
    DLN("Up for "+String(millis())+" ms, going to sleep forever because not paired so far... \n");       
    gotoInfiniteSleep();
    return;
  }

  // start ESPNow
  simpleEspConnection.begin();

  simpleEspConnection.onPairingFinished(&OnPairingFinished);  
  simpleEspConnection.setPairingBlinkPort(LEDPIN);  
  simpleEspConnection.onSendError(&OnSendError);    
  if(configuration.containsKey("gateway") && !pairingOngoing)
  {
    simpleEspConnection.setServerMac(configuration.get("gateway")); 
  }  
  simpleEspConnection.onSendDone(&OnSendDone);
  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onMessage(&OnMessage);  
  
  if(pairingOngoing)
  {
    avoidSleeping = true;
    DLN("Start pairing");    
    ESP.eraseConfig();
    if(!simpleEspConnection.startPairing(300))
    {
      DLN("Start pairing failed!");      
    }
  }
  else
  {
    prepareend = 1000 * atoi(configuration.get("preparetime"));      

    if(strcmp(lastVersionNumber.c_str(), currVersion.c_str()) != 0)
    {    
      sendSetup();
      configuration.storeVersionNumber();
    }      
  }

  DV(configuration.get("devicetype"));

  if(strcmp(configuration.get("devicetype"), "UNKNOWN") == 0)  
  {
    sendMessageWithChecksum("$F$");            
  }

  clockmillis = millis();  
}

void getMeasurementData()
{
  bool sendJson = !strcmp(configuration.getWlanParameter("MQTTsendjson"), "true") ? true : false;
  String jsonSendBuffer;

  if(configuration.containsKey("gateway") || gatewayMode)
  {      
    if(dev != NULL)
    {
      SensorData* sd = dev->readParameter();
      lastMessage = F("\"");
  
      if(sd != NULL)
      {
        String reply;
        if(sendJson)
          jsonSendBuffer = "{";
          
        for(int i = 0; i<sd->size; i++)
        {
          reply = sd->di[i].nameI+":"+sd->di[i].valueI;

          lastMessage += "(" + String(i)+ ")" +sd->di[i].nameI+"="+sd->di[i].valueI+"&#13;&#10;";

//          DV(reply);
          

          if(gatewayMode)
          {
            if(sendJson)
            {
              jsonSendBuffer += "\""+sd->di[i].nameI+"\":\""+sd->di[i].valueI+"\",";
            }
            else
            {
              mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/sensor/"+sd->di[i].nameI).c_str(), sd->di[i].valueI.c_str()); 
            }

            // send to cloud
            if(strcmp(configuration.getCloudParameter("enabled"),"true") == 0)
            {
              if(cloudConnected && cloudIsDeviceRegistered)
                cloudAddMeasurement(deviceName.c_str(), sd->di[i].nameI.c_str(), sd->di[i].valueI.c_str());
            }
          }
          else   
          {             
            sendMessageWithChecksum((char *)("$D$"+reply).c_str());                       
          }
        }

        if(sendJson)
        {
          jsonSendBuffer = jsonSendBuffer.substring(0, jsonSendBuffer.length()-1);
          jsonSendBuffer += "}";
          mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/sensor").c_str(), jsonSendBuffer.c_str()); 
        }
        
        delete sd;
        sd = NULL;  
      }

      lastMessage += F("\"");      

      // send message about last measurement;
      if(!gatewayMode)
      {
        sendMessageWithChecksum("$F$");                       
        
        if(!preparing)
          lastIntervalTime = millis();            
      }
    }
  }  
}

void handleGatewayLoop()
{
  if(!apConnectingOngoing)    
    server.handleClient();
  
  if(apConnectingOngoing)
  {
    if (WiFi.status() == WL_CONNECTED) 
    {
      apConnectingOngoing = false;
      DLN("Connected to AP. Starting Webserver...");

      controlConnectionBlinker(false);

      DV(WiFi.localIP());
      DV(WiFi.dnsIP());
      DV(WiFi.gatewayIP());
      DV(WiFi.subnetMask());
      
      
      server.on(F("/"), handleRoot);
      server.on(F("/reboot"), handleReboot);
      server.on(F("/general"), handleRoot);
      server.on(F("/settings"), handleSettings);
      server.on(F("/gateway"), handleGateway);
      server.on(F("/NTP"), handleNTP);
      server.on(F("/cloud"), handleCloud);

      server.on(F("/j.js"), handleJs);      
      server.on(F("/json"), handleJson);
      
      server.onNotFound(handleNotFound);
      server.begin();     

      if(strcmp(configuration.getCloudParameter("enabled"),"true") == 0 && !cloudConnected)
      {
        DLN("Will start cloud connection");
        cloudConnected = cloudLogin(configuration.getCloudParameter("user"), configuration.getCloudParameter("password"));        
        cloudIsDeviceRegistered = cloudIsDeviceRegisteredHere(deviceName.c_str());
      }         
    } 
    else
    {
/*      if(millis() > apConnectionStartTime + 10000)
      {
        DLN("Connection timeout. Will start a local AP to reconfigure.");

        configuration.storeLastRebootInfo("connectiontimeout");

        delay(500);
        ESP.restart();
      }*/
    }
  }

  if(WiFi.status() == WL_CONNECTED)
  {        
    if(strcmp(configuration.getWlanParameter("MQTTenabled"), "true") == 0)
    {  
      if(!mqttclient.connected())
      {
        if(millis() > mqttNextTry)
        {
          if(!mqttreconnect())
          {
            mqttNextTry = millis() + 5000;
            return;
          }
        }
      }
      else
        mqttclient.loop();
    }
  }
  else
    return;
  
  if(setNewDeviceType)
  {
    initDeviceType(newDeviceType.c_str(), true);
    setNewDeviceType = false;
    newDeviceType = "";
  
    configuration.store();        
    DLN("vor sendSetup");
    sendSetup();    
    DLN("nach sendSetup");
    configuration.storeLastRebootInfo("devicechanged");

    delay(500);
    ESP.restart();
  }

  if(firmwareUpdate)
    upgradeFirmware();


  // only if interval is reached or if preparing ongoing
  if(millis() - lastIntervalTime < (atoi(configuration.get("interval")) - atoi(configuration.get("preparetime"))) *1000 && !firstMeasurement)
    return;

  firstMeasurement = false;

  if(preparestart == 0 && atoi(configuration.get("preparetime")) > 0)
  {
    preparestart = millis();
    preparing = false;
  }
      
  handleBatteryCheck();

  if(!isDeviceInitialized)
  {
    if(strcmp(configuration.get("devicetype"), "UNKNOWN") != 0)
    {
      initDeviceType(configuration.get("devicetype"), false);
    }
    
    isDeviceInitialized = true;
  }  

  if(dev != NULL)
  {
    if(preparestart > 0 && !preparing)
    {
      dev->prepare();
      DLN("Start prepare");
      preparing = true;
    }
  }  

  if(millis() > preparestart + atoi(configuration.get("preparetime")) * 1000)
  {
    preparing = false;
    preparestart = 0;

    getMeasurementData();
  }

  if(!preparing)
    lastIntervalTime = millis();  
}

void handleESPNowLoop()
{
  if((!firmwareUpdate && configuration.containsKey("gateway")) || pairingOngoing)
    sendBufferFilled = simpleEspConnection.loop();


  if(setNewDeviceType)
  {
    initDeviceType(newDeviceType.c_str(), true);
    setNewDeviceType = false;
    newDeviceType = "";

    configuration.store();        
    DLN("vor sendSetup");
    sendSetup();    
    DLN("nach sendSetup");

    delay(100);
    ESP.restart();
    delay(500);
  }

  if(firmwareUpdate)
    upgradeFirmware();

  if (finalMeasurementDone)
    return;

  if(!configuration.containsKey("gateway") || String(configuration.get("gateway")) == "")
    return;

  else if(atoi(configuration.get("interval")) < 0)
    sleepEnabled = true;    
  else if(atoi(configuration.get("processtime")) + atoi(configuration.get("preparetime")) >= atoi(configuration.get("interval")))   
    sleepEnabled = false;
  else
    sleepEnabled = true;  

  firstMeasurement = false;

  if(preparestart == 0 && atoi(configuration.get("preparetime")) > 0)
  {
    preparestart = millis();
    DV(preparestart);
    preparing = false;
  }

  if(!isDeviceInitialized)
  {
    if(strcmp(configuration.get("devicetype"), "UNKNOWN") != 0)
    {
      initDeviceType(configuration.get("devicetype"), false);
    }
    
    isDeviceInitialized = true;
  }  

  if(dev != NULL && isDeviceInitialized)
  {
    if(preparestart > 0 && !preparing)
    {
      dev->prepare();
      DLN("Start prepare");
      preparing = true;
    }

    if(millis() > preparestart + atoi(configuration.get("preparetime")) * 1000 && !finalMeasurementDone)
    {
      preparing = false;
      preparestart = 0;

      getMeasurementData();

      handleBatteryCheck();
      
      finalMeasurementDone = true;
    }
  }  

  if(preparing)
  {
    if(millis() < atoi(configuration.get("preparetime")) *1000 + lastIntervalTime)
      return;
  }
}

void loop() 
{
  if(writeConfiguration)
  {
    writeConfiguration = false;
    configuration.store();

    sendSetup();
  }

  if(checkAPModeRequest())
  {
    if(pairingOngoing)
      simpleEspConnection.endPairing();

    pairingOngoing = false;
    configurationMode = true;
    startConfigurationAP();
    
    return;    
  }

  if(pairingOngoing)
    return;  
  
  if(configurationMode)
  {
    server.handleClient();
    return;  
  }

  if(gatewayMode)
  {
    handleGatewayLoop();
    return;
  }
  else
  {
    handleESPNowLoop();
  }
  
  
  if(!sleepEnabled)
    return;
  
  if(forceSleep || (lastIntervalTime > 0 && millis() > MAXCONTROLWAIT+lastIntervalTime))
  {
    if(!preparing && !setNewDeviceType && simpleEspConnection.isSendBufferEmpty())
    {      
      if(!avoidSleeping)
      {
        if(atoi(configuration.get("interval")) < 0)    
          gotoInfiniteSleep();
        else
          gotoSleep();
      }
    }
  }  

  if(initReboot && !writeConfiguration)
  {
    ESP.restart();
    delay(500);

    return;
  }  
}

void gotoInfiniteSleep()
{
  int sleepSecs;
  DLN("Up for "+String(millis())+" ms, going down for sleep and hope will not be waked up by myself... \n"); 

  if(!configuration.containsKey("interval"))
  {
    configuration.resetConfiguration();
    ESP.restart();
    delay(500);

    return;
  }

  sleepSecs = atoi(configuration.get("interval"))*(-1);
  if(atoi(configuration.get("sensorpowerpin")) != 99)
  {      
    digitalWrite(atoi(configuration.get("sensorpowerpin")),LOW);          
    pinMode(atoi(configuration.get("sensorpowerpin")), INPUT);      
  }
  
  ESP.deepSleep(sleepSecs * 1000000, RF_NO_CAL);
  
  delay(100);  
}

void gotoSleep() 
{  
  delete dev;  
  int sleepSecs;

  if(strcmp(configuration.get("devicetype"), "UNKNOWN") == 0) // goto sleep just for 5 seconds and flash 2 times
  {
    sleepSecs = 5;
#ifdef LEDPIN
    pinMode(LEDPIN, OUTPUT);

    digitalWrite(LEDPIN, LOW);
    delay(100);
    digitalWrite(LEDPIN, HIGH);
    delay(100);
    digitalWrite(LEDPIN, LOW);
    delay(100);
    digitalWrite(LEDPIN, HIGH);
#endif
  }
  else
  {
    sleepSecs = atoi(configuration.get("interval")) - atoi(configuration.get("preparetime"));
  }

  DLN("Up for "+String(millis())+" ms, going to sleep for "+String(sleepSecs)+" secs... \n"); 

  if(sleepSecs > 0)
  {
    if(atoi(configuration.get("sensorpowerpin")) != 99)
    {      
      digitalWrite(atoi(configuration.get("sensorpowerpin")),LOW);          
      pinMode(atoi(configuration.get("sensorpowerpin")), INPUT);      
    }
    
    ESP.deepSleep(sleepSecs * 1000000, RF_NO_CAL);
  }
  
  delay(100);
}
