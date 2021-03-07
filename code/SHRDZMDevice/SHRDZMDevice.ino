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
char menuContextBuffer[4200];

/// Configuration Webserver
void startConfigurationAP()
{
  configurationMode = true;
    
  String APName = "SHRDZMDevice-"+deviceName;
  WiFi.hostname(APName.c_str());        
  WiFi.softAP(APName);     

  DLN("Start configuration AP...");
  
  server.on("/", handleRoot);
  server.on("/reboot", handleReboot);
  server.on("/general", handleRoot);
  server.on("/settings", handleSettings);
  server.on("/gateway", handleGateway);
  server.onNotFound(handleNotFound);
  server.begin();
  
  configurationBlinker.attach(0.2, changeConfigurationBlinker);
}

char* getWebsite(char* content)
{  
  int len = strlen(content);

  DLN("Content len = "+String(len));

  sprintf(websideBuffer,  
"<!DOCTYPE html>\
<html>\
<head>\
<link rel=\"icon\" type=\"image/svg+xml\" href=\"https://shrdzm.pintarweb.net/Logo_min_green.svg\" sizes=\"any\">\
<style>\
body {\
  font-family: Arial, Helvetica, sans-serif;\
}\
\
hr\
{ \
  display: block;\
  margin-top: 0.5em;\
  margin-bottom: 0.5em;\
  margin-left: auto;\
  margin-right: auto;\
  border-style: inset;\
  border-width: 1px;\
}\
ul \
{\
list-style-type: none;\
  margin: 0;\
  padding: 0;\
  width: 150px;\
  background-color: #f1f1f1;\
  position: fixed;\
  height: 100%;\
  overflow: auto;\
}\
\
li a {\
  display: block;\
  color: #000;\
  padding: 8px 16px;\
  text-decoration: none;\
}\
\
li a.active {\
  background-color: #4CAF50;\
  color: white;\
}\
\
li a:hover:not(.active) {\
  background-color: #555;\
  color: white;\
}\
label.h2 {\
  font-weight: bold;\
  font-size: 150%;\
  width: 100%;\
  margin-bottom: 1em;\
}\
input,\
label {\
  float: left;\
  width: 40%;\
  margin-left: 1.5;\
  padding-left: 5px;\
}\
label {\
  display: inline-block;\
  width: 7em;\
}\
input {\
  margin: 0 0 1em .2em;\
  padding: .2em .5em;\
  background-color: #fffbf0;\
  border: 1px solid #e7c157;\
}\
label.input {\
  text-align: right;\
  margin-left: 10.5;\
  padding-left: 80px;\
  line-height: 1.5;\
}\
input.factoryresetbutton, textarea {\
background: cyan;\
border: 2px solid red;\
color: black;\
}\
button {\
  margin-top: 1.5em;\
  width: 30%;\
  border-radius: 10px;\
}\
.factoryresetbutton\
  background-color: Red;\
  border: 2px solid black;\
  border-radius: 5px;\
}\
.submitbutton {\
  background-color: Gainsboro;\
  border: 1px solid black;\
  border-radius: 5px;\
}\
.main {\
  margin-left: 200px;\
  margin-bottom: 30px;\
}\
</style>\
<title>SHRDZMDevice - %s</title>\
</head>\
<body>\
\
<ul>\
  <li>\
    <a class='active' href='#home'>SHRDZMDevice<br/>\
      <font size='2'>%s</font>\
    </a></li>\
  <li><a href='./general'>General</a></li>\
  <li><a href='./settings'>Settings</a></li>\
  <li><a href='./gateway'>Gateway</a></li>\
  <li><a href='./about'>About</a></li>\
  <li><a href='./reboot'>Reboot</a></li>\
  <br/>\
  <li><font size='2' color='blue'><a href='http://shrdzm.com/' target='_blank'>\
  <img alt='SHRDZM' src='https://shrdzm.pintarweb.net/Logo.svg' width='60'>\
  Home</a></font></li>\
  <br/><br/><br/>\
  <li><center>&copy;&nbsp;<font size='2' color='darkgray'>Erich O. Pintar</font></center></li>\
  <br/><br/>\
</ul>\
\
<div class='main'>\
  %s\
</div>\
</body>\
</html>\
  ", deviceName.c_str(), deviceName.c_str(), content);

  return websideBuffer;
}

void handleRoot() 
{
  char content[2500];

  if(server.hasArg("factoryreset"))
  {
    if(String(server.arg("factoryreset")) == "true") // factory reset was pressed
    {
      snprintf(content, 300,
      "<!DOCTYPE html>\
      <html>\
      <head>\
      </head>\
      <body>\
      <h1>Factory Reset was pressed. SHRDZMDevice will be restarted with default configuration.</h1>\
      </body>\
      </html>\
      ");
        
      server.send(200, "text/html", content);

#ifdef LITTLEFS
      LittleFS.format();
#else
      SPIFFS.format();
#endif

      delay(2000);
      
      ESP.restart();        
    }
  }

  if(server.hasArg("upgrade") && server.hasArg("upgradepath"))
  {
    if(String(server.arg("upgrade")) == "true") // Upgrade was pressed
    {
      DLN("Upgrade from "+String(server.arg("upgradepath"))+" will be started...");
      if(updateFirmwareByMQTT(server.arg("upgradepath")))
      {        
        firmwareUpdate = true;

        snprintf(content, 300,
        "<!DOCTYPE html>\
        <html>\
        <head>\
        </head>\
        <body>\
        <h1>Upgrade started. SHRDZMDevice will reboot after upgrade.</h1>\
        </body>\
        </html>\
        ");
          
        server.send(200, "text/html", content);  
        return;      
      }
    }    
  }
  
  String informationTable = "<br>";  
  String upgradeText = "";

  if(gatewayMode)
  {
    upgradeText = "<br/><br/><input type='hidden' id='upgrade' name='upgrade' value='false'/><br/>\
        <input class='factoryresetbutton' type='submit' onclick='submitFormUpgrade()' value='OTA Upgrade' />\
        <input type='text' id='upgradepath' name='upgradepath' size='35' value='http://shrdzm.pintarweb.net/upgrade.php'>\
        <br/>";
  }

  informationTable += "Firmware Version : "+ver+"-"+ESP.getSketchMD5()+"<br><br>";
  informationTable += "Device Type : "+String(configuration.get("devicetype"))+"<br>";
  informationTable += "Chip ID : "+String(ESP.getChipId())+"<br>";
  informationTable += "Gateway Mode : "+String(configuration.getWlanParameter("enabled"))+"<br>";
  informationTable += "MQTTTopic Gateway : SHRDZM/"+String(configuration.get("gateway"))+"<br>";
  informationTable += "MQTTTopic Device : SHRDZM/"+String(configuration.get("gateway"))+"/"+deviceName+"<br>";
  informationTable += "MQTTTopic Sensor : SHRDZM/"+String(configuration.get("gateway"))+"/"+deviceName+"/sensor/<br><br>";

  if(WiFi.localIP().toString() != "(IP unset)")
  {
    informationTable += "IP : "+WiFi.localIP().toString()+"<br>";
    informationTable += "DNS : "+WiFi.dnsIP().toString()+"<br>";
    informationTable += "Gateway : "+WiFi.gatewayIP().toString()+"<br>";
    informationTable += "Subnet : "+WiFi.subnetMask().toString()+"<br>";
  }

  sprintf(content,  
      "<h1>General</h1>\
      <img alt='SHRDZM' src='https://shrdzm.pintarweb.net/logo_200.png' width='200'>\
      <br /><br /><br /><br />\
      %s\
      <br/><br/>\
      <form method='post' id='factoryReset'>\
      <input type='hidden' id='factoryreset' name='factoryreset' value='false'/>\
      <input class='factoryresetbutton' type='submit' onclick='submitFormFactoryReset()' value='Factory Reset!' />\
      %s\
      <script>\
       function submitFormFactoryReset()\
       {\
          document.getElementById('factoryreset').value = 'true';\
       }\
       function submitFormUpgrade()\
       {\
          document.getElementById('upgrade').value = 'true';\
       }\
      </script>\
      </form>\
      ",
      informationTable.c_str(),
      upgradeText.c_str()
  );  

  char * temp = getWebsite(content);
  server.send(200, "text/html", temp);
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleReboot() 
{
  char temp[300];
  
  snprintf(temp, 300,
  "<!DOCTYPE html>\
  <html>\
  <head>\
  <meta http-equiv='refresh' content='20; url=/'>\
  </head>\
  <body>\
  <h1>Please wait. Will reboot in 20 seconds...</h1>\
  </body>\
  </html>\
  ");


  server.send(200, "text/html", temp);

  delay(2000);
  
  ESP.restart();  
}

void handleSettings()
{
//  char content[2700];
  String deviceBuffer = "<option></option>";
  String parameterBuffer = "";
  String deviceType;
  String b;
  int loop = 0;
  JsonObject deviceParameter;
  SensorData* initialSettings = NULL;

  if(settingDev != NULL)
  {
    free(settingDev);
    settingDev = NULL;
  }
  
  // check selected device
  if(server.hasArg("devices"))
    deviceType = server.arg("devices");
  else
    deviceType = configuration.get("devicetype");       

  if(deviceType != "" && deviceType != "UNKNOWN" && settingDev == NULL)
  {
    settingDev = createDeviceObject(deviceType.c_str());        
    settingDev->initialize();
  }
    
  // check if save was pressed
  if(server.hasArg("save"))
  {
    if(String(server.arg("save")) == "true") // now to save the parameter to the configuration
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
        configuration.removeAllDeviceParameter();
        
        for (JsonPair kv : deviceParameter)
        {
          if(server.hasArg(kv.key().c_str()))
          {
            configuration.setDeviceParameter(kv.key().c_str(), server.arg(kv.key().c_str()).c_str());
          }
        }    
      }  

      writeConfiguration = true;          
    }
  }  

  if(settingDev != NULL)
  {
    if(strcmp(configuration.get("devicetype"), deviceType.c_str()) == 0)    
//    if(deviceType == configuration.get("devicetype"))
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
      if(String(kv.key().c_str()) != "device" && String(kv.key().c_str()) != "wlan" && String(kv.key().c_str()) != "devicetype")
      {
        parameterBuffer += "<br/><br/><div><label for='"+String(kv.key().c_str())+"'>"+String(kv.key().c_str())+"</label>";        
        if(initialSettings != NULL && initialSettings->getDataItem(kv.key().c_str()) != "")
        {
          parameterBuffer += "<input type='text' id='"+String(kv.key().c_str())+"' name='"+String(kv.key().c_str())+"' size='10' value='"+String(initialSettings->getDataItem(kv.key().c_str()))+"'></div>";
        }
        else        
        {
          parameterBuffer += "<input type='text' id='"+String(kv.key().c_str())+"' name='"+String(kv.key().c_str())+"' size='10' value='"+String(kv.value().as<char*>())+"'></div>";
        }
      }
    }    

    // Show device parameter
    if(!deviceParameter.isNull())
    {
      parameterBuffer += "<br/>";
      for (JsonPair kv : deviceParameter)
      {
        parameterBuffer += "<br/><br/><div><label for='"+String(kv.key().c_str())+"'>"+String(kv.key().c_str())+"</label>";        
        {
          parameterBuffer += "<input type='text' id='"+String(kv.key().c_str())+"' name='"+String(kv.key().c_str())+"' size='10' value='"+String(kv.value().as<char*>())+"'></div>";
        }
      }    
    }    
  } 
    
  // Fill select box
  while(true)
  {
    b = getValue(SUPPORTED_DEVICES, ',', loop++);
    if(b != "")
    {
      if(b == deviceType)
        deviceBuffer += "<option selected>"+b+"</option>";
      else      
        deviceBuffer += "<option>"+b+"</option>";
    }
    else
      break;
  }

  sprintf(menuContextBuffer,  
      "<h1>Settings</h1><p><strong>Configuration</strong><br /><br />\
      <form method='post' id='settingsForm'>\
      <label>Device Type :\
        <select name='devices' onchange='this.form.submit()'>\
        %s\
        </select>\
      </label>\
      <br/><br/>\
      %s\
      <br/><br/>\
      <input type='hidden' id='save' name='save' value='false'/>\
      <input class='submitbutton' type='submit' onclick='submitForm()' value='Save Configuration!' />\
      <script>\
       function submitForm()\
       {\
          document.getElementById('save').value = 'true';\
       }\
      </script>\
      </form>\
      ",
      deviceBuffer.c_str(),
      parameterBuffer.c_str()
  );  

  char * temp = getWebsite(menuContextBuffer);
  DV(writeConfiguration);
  DLN("after getWebsite size = "+String(strlen(temp)));
  
  server.send(200, "text/html", temp);  
}

void handleGateway()
{
//  char content[2300];

  if(server.args() != 0)
  {
    if( server.arg("wlanenabled") == "1")
    {
      configuration.setWlanParameter("enabled", "true");
      configuration.set("gateway", (char *)deviceName.c_str());
    }
    else
      configuration.setWlanParameter("enabled", "false");

    if(server.hasArg("ssid"))
      configuration.setWlanParameter("ssid", server.arg("ssid").c_str());
    else
      configuration.setWlanParameter("ssid", "");
        
    if(server.hasArg("password"))
      configuration.setWlanParameter("password", server.arg("password").c_str());
    else
      configuration.setWlanParameter("password", "");    

    if(server.hasArg("ip"))
      configuration.setWlanParameter("ip", server.arg("ip").c_str());
    else
      configuration.setWlanParameter("ip", "");    
    
    if(server.hasArg("dns"))
      configuration.setWlanParameter("dns", server.arg("dns").c_str());
    else
      configuration.setWlanParameter("dns", "");    
    
    if(server.hasArg("gateway"))
      configuration.setWlanParameter("gateway", server.arg("gateway").c_str());
    else
      configuration.setWlanParameter("gateway", "");    
    
    if(server.hasArg("subnet"))
      configuration.setWlanParameter("subnet", server.arg("subnet").c_str());
    else
      configuration.setWlanParameter("subnet", "");    
    
    if(server.hasArg("MQTTbroker"))
      configuration.setWlanParameter("MQTTbroker", server.arg("MQTTbroker").c_str());
    if(server.hasArg("MQTTport"))
      configuration.setWlanParameter("MQTTport", server.arg("MQTTport").c_str());
    if(server.hasArg("MQTTuser"))
      configuration.setWlanParameter("MQTTuser", server.arg("MQTTuser").c_str());
    if(server.hasArg("MQTTpassword"))
      configuration.setWlanParameter("MQTTpassword", server.arg("MQTTpassword").c_str());

    writeConfiguration = true;    
  }
   
  sprintf(menuContextBuffer,  
      "<h1>Gateway</h1><p><strong>Configuration</strong><br /><br />\
      WLAN Settings if Device acts as it's own gateway.\
      <br/><br/>\
      <form method='post'>\
      <input type='checkbox' id='wlanenabled' name='wlanenabled' value='1' %s/>\
      <input type='hidden' name='wlanenabled' value='0' />\
      <div><label for='wlanenabled'>Device should act as it's own gateway</label></div><br/>\
      <br/><br/>\
      <hr/>\
      <div><input type='text' id='ssid' name='ssid' placeholder='SSID' size='50' value='%s'>\
      <p><label for='ssid'>SSID</label></p></div><br/>\
      <br/>\
      <div><input type='password' id='password' name='password' placeholder='Password' size='50' value='%s'>\
      <label for='password'>Password</label></div><br/><br/>\
      <div><input type='checkbox' onclick='showWLANPassword()'>Show Password\
      </div><br/>\
      <div><input type='text' id='ip' name='ip' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
      <label for='ip'>Static IP</label></div><br/>\
      <br/>\
      <div><input type='text' id='dns' name='dns' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
      <label for='ip'>DNS</label></div><br/>\
      <br/>\
      <div><input type='text' id='gateway' name='gateway' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
      <label for='gateway'>Gateway</label></div><br/>\
      <br/>\
      <div><input type='text' id='subnet' name='subnet' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
      <label for='gateway'>Subnet</label></div><br/>\
      <br/><br/>\
      <div><input type='text' id='MQTTbroker' name='MQTTbroker' placeholder='MQTT Broker' size='50' value='%s'>\
      <label for='MQTTbroker'>MQTT Broker</label></div><br/>\
      <br/>\
      <div><input type='text' id='MQTTport' name='MQTTport' placeholder='MQTT Port' size='50' value='%s'>\
      <label for='MQTTport'>MQTT Port</label></div><br/>\
      <br/>\
      <div><input type='text' id='MQTTuser' name='MQTTuser' placeholder='MQTT User' size='50' value='%s'>\
      <label for='MQTTuser'>MQTT User</label></div><br/>\
      <br/>\
      <div><input type='text' id='MQTTpassword' name='MQTTpassword' placeholder='MQTT Password' size='50' value='%s'>\
      <label for='MQTTpassword'>MQTT Password</label></div><br/>\
      <br/><br />\
      <input class='submitbutton' type='submit' value='Save Configuration!' />\
      <script>\
      function showWLANPassword() {\
        var x = document.getElementById('password');\
        if (x.type === 'password') {\
          x.type = 'text';\
        } else {\
          x.type = 'password';\
        }\
      }\
      </script>\
      </form>\
      "
      ,
      String(configuration.getWlanParameter("enabled")) == "true" ? "checked" : "",
      configuration.getWlanParameter("ssid"),
      configuration.getWlanParameter("password"),
      configuration.getWlanParameter("ip"),
      configuration.getWlanParameter("dns"),
      configuration.getWlanParameter("gateway"),
      configuration.getWlanParameter("subnet"),
      configuration.getWlanParameter("MQTTbroker"),
      configuration.getWlanParameter("MQTTport"),
      configuration.getWlanParameter("MQTTuser"),
      configuration.getWlanParameter("MQTTpassword") 
  );  

  char * temp = getWebsite(menuContextBuffer);

  DLN("after getWebsite size = "+String(strlen(temp)));
  server.send(200, "text/html", temp);
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
        if(splitter->getItemAtIndex(1) == "configuration")
        {
          sendSetup();
        }
      }
    }
    else if(itemCount == 3)
    {
      if(splitter->getItemAtIndex(1) == "upgrade")
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
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,5) == "pair ")
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
                       
  mqttclient.setServer(configuration.getWlanParameter("MQTTbroker"), 
                       atoi(configuration.getWlanParameter("MQTTport")));

  mqttclient.setCallback(mqttcallback);
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
  else if(strcmp(deviceType, "IM350") == 0)
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
    simpleEspConnection.sendMessage((char *)"$I$");
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
        simpleEspConnection.sendMessage((char *)reply.c_str());
      
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
          simpleEspConnection.sendMessage((char *)reply.c_str());    
      }      
    }        
  }

  String s = String("$V$")+ver+"-"+ESP.getSketchMD5();

  if(!gatewayMode)
    simpleEspConnection.sendMessage((char *)s.c_str());
  else
    mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/version").c_str(), (ver+"-"+ESP.getSketchMD5()).c_str());
  

  // send supported devices
  s = String("$X$")+String(SUPPORTED_DEVICES);
  if(!gatewayMode)
    simpleEspConnection.sendMessage((char *)s.c_str());  
  else
    mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/sensors").c_str(), (String(SUPPORTED_DEVICES)).c_str());

  avoidSleeping = false;    
}

// Upgrade firmware via inbuild gateway
bool updateFirmwareByMQTT(String message)
{
  host = message;
  
  if(host.substring(0,7) != "http://")
  {
    DLN("Upgrade : only http address supported! ");
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

  DLN("setConfig Name="+pname+", Value="+pvalue);

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
    forceSleep= true;
    return;
  }
  
  if(String((char *)message) == "$S$") // ask for settings
  {
    sendSetup();
  }
  else if(String((char *)message) == "$PING$") // ping
  {
    if(configuration.containsKey("gateway"))
    {
      simpleEspConnection.sendMessage("$PING$");
    }
      
    return;
  }
  else if(String((char *)message).substring(0,5) == "$SDT$") // set device type
  {
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
    t_httpUpdate_return ret = ESPhttpUpdate.update(host, 80, url, versionStr);    
    
    switch (ret) 
    {
      case HTTP_UPDATE_FAILED:
     //   Serial.printf("HTTP_UPDATE_FAILD Error (%d):  sendUpdatedVersion %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
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
  else if(strcmp(deviceType, "IM350") == 0)
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

  // set device name
  WiFi.mode(WIFI_STA);  
  uint8_t pmac[6];
  WiFi.macAddress(pmac);
  deviceName = macToStr(pmac);
  
  deviceName.replace(":", "");
  deviceName.toUpperCase();

//  DV(deviceName);

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
//  DLN("Last Reboot Info = "+lastRebootInfo);    

  configuration.storeLastRebootInfo("normal");
  
  if(lastRebootInfo == "connectiontimeout" && !pairingOngoing)
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
    DLN("Send $F$");
    simpleEspConnection.sendMessage("$F$");
  }

  clockmillis = millis();  
}

void getMeasurementData()
{
  if(configuration.containsKey("gateway") || gatewayMode)
  {      
    if(dev != NULL)
    {
      SensorData* sd = dev->readParameter();
  
      if(sd != NULL)
      {
        String reply;
        
        for(int i = 0; i<sd->size; i++)
        {
          reply = sd->di[i].nameI+":"+sd->di[i].valueI;

          DV(reply);

          if(gatewayMode)
          {
            DLN("MQTT Publish data "+String((String(MQTT_TOPIC)+"/"+deviceName+"/sensor/"+sd->di[i].nameI).c_str()));
            mqttclient.publish((String(MQTT_TOPIC)+"/"+deviceName+"/sensor/"+sd->di[i].nameI).c_str(), sd->di[i].valueI.c_str()); 
          }
          else                
            simpleEspConnection.sendMessage((char *)("$D$"+reply).c_str());          
        }
        delete sd;
        sd = NULL;
      }

      // send message about last measurement;
      if(!gatewayMode)
      {
        simpleEspConnection.sendMessage("$F$");
        DLN("Send $F$");
        
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

      DV(WiFi.localIP());
      DV(WiFi.dnsIP());
      DV(WiFi.gatewayIP());
      DV(WiFi.subnetMask());
      
      
      server.on("/", handleRoot);
      server.on("/reboot", handleReboot);
      server.on("/general", handleRoot);
      server.on("/settings", handleSettings);
      server.on("/gateway", handleGateway);
      server.onNotFound(handleNotFound);
      server.begin();        
    } 
    else
    {
      if(millis() > apConnectionStartTime + 10000)
      {
        DLN("Connection timeout. Will start a local AP to reconfigure.");

        configuration.storeLastRebootInfo("connectiontimeout");

        delay(500);
        ESP.restart();
      }
    }
  }

  if(WiFi.status() == WL_CONNECTED)
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

  // get measurement data
    loopDone = dev->loop();
    if(dev->isNewDataAvailable())
    {
//      initDeviceType(
//      initDeviceType(configuration.get("devicetype"), true, false);
      
      getMeasurementData();
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

  handleBatteryCheck();

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

      DLN("Will start measurement");
      getMeasurementData();
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
    if(!preparing && !setNewDeviceType && simpleEspConnection.isSendBufferEmpty() && !avoidSleeping)
    {      
      if(atoi(configuration.get("interval")) < 0)    
        gotoInfiniteSleep();
      else
        gotoSleep();
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
  if(atoi(configuration.get("sensorpowerpin")) != 99)
  {
    digitalWrite(atoi(configuration.get("sensorpowerpin")),LOW); 
    pinMode(atoi(configuration.get("sensorpowerpin")), INPUT);                   
  }

  DLN("Up for "+String(millis())+" ms, going down for infinit sleep... \n"); 
  
  ESP.deepSleep(0);  
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
