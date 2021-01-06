/*
  SHRDZMGateway

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 28 August 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#include "config/config.h"
#include <ArduinoJson.h>
#include "SimpleEspNowConnection.h"
#include "SetupObject.h"
#include "MQTTBufferObject.h"
#include "StringSplitter.h"
#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>

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

ESP8266WebServer server(80);

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
bool simEnabled = false;
bool accesspointmodeEnabled = false;
int ap_pin = -1;

ESP8266WiFiMulti WiFiMulti;


String subscribeTopicSet;
String subscribeTopicConfig;
String APName;

SetupObject setupObject;
MQTTBufferObject mqttBufferObject;

// AP WebServer
char* getWebsite(char* content)
{  
  int len = strlen(content);
  
  char *temp = (char *) malloc (1400+len);

#ifdef DEBUG
  Serial.println("Handle Root");
  Serial.println("Content len = "+String(len));
#endif

  snprintf(temp, 1400+len,  
"<!DOCTYPE html>\
<html>\
<head>\
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
.main {\
  margin-left: 200px;\
  margin-bottom: 30px;\
}\
</style>\
<title>SHRDZMGateway - %s</title>\
</head>\
<body>\
\
<ul>\
  <li>\
    <a class='active' href='#home'>SHRDZMGateway<br/>\
      <font size='2'>%s</font>\
    </a></li>\
  <li><a href='./general'>General</a></li>\
  <li><a href='./settings'>Settings</a></li>\
  <li><a href='./about'>About</a></li>\
  <li><a href='./reboot'>Reboot</a></li>\
  <br/>\
  <li><a href='./deleteconfig'>Delete Config</a></li>\  
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

  return temp;
}

void handleRoot() 
{
  char * temp = getWebsite("<h1>General</h1>General Information");

  server.send(200, "text/html", temp);

  free(temp);
}

void handleSettings()
{
  bool writeConfiguration = false;
  
  char content[2600];

  if(server.args() != 0)
  {
    if(server.hasArg("ssid"))
      configdoc["wlan"]["ssid"] = server.arg("ssid");  
    else
      configdoc["wlan"]["ssid"] = "";  
        
    if(server.hasArg("ssidpassword"))
      configdoc["wlan"]["password"] = server.arg("ssidpassword");      
    else
      configdoc["wlan"]["password"] = "";      
    
    if(server.hasArg("sim800"))
    {
      if( server.arg("sim800") == "1")
      {
        configdoc["sim800"]["enabled"] = "true";
      }
      else
      {
        configdoc["sim800"]["enabled"] = "false";
      }
    }
    else
    {
      configdoc["sim800"]["enabled"] = "false";
    }

    if(server.hasArg("pin"))
      configdoc["sim800"]["pin"] = server.arg("pin");      
    if(server.hasArg("apn"))
      configdoc["sim800"]["apn"] = server.arg("apn");      
    if(server.hasArg("user"))
      configdoc["sim800"]["user"] = server.arg("user");      
    if(server.hasArg("password"))
      configdoc["sim800"]["password"] = server.arg("password");      
    if(server.hasArg("MQTTbroker"))
      configdoc["sim800"]["MQTTbroker"] = server.arg("MQTTbroker");      
    if(server.hasArg("MQTTport"))
      configdoc["sim800"]["MQTTport"] = server.arg("MQTTport");      
    if(server.hasArg("MQTTuser"))
      configdoc["sim800"]["MQTTuser"] = server.arg("MQTTuser");      
    if(server.hasArg("MQTTpassword"))
      configdoc["sim800"]["MQTTpassword"] = server.arg("MQTTpassword");      

    writeConfiguration = true;    
  }
  
  snprintf(content, 2600,  
      "<h1>Settings</h1><p><strong>Configuration</strong><br /><br />\
      <p>WLAN Settings only needed if OTA is used.</p>\
      <br/><br/>\
      <form method='post'>\
      <input type='text' id='ssid' name='ssid' placeholder='SSID' size='50' value='%s'>\
      <label for='ssid'>SSID</label><br/>\
      <br/>\
      <input type='password' id='ssidpassword' name='ssidpassword' placeholder='Password' size='50' value='%s'>\
      <label for='ssidpassword'>Password</label><br/>\
      <input type='checkbox' onclick='showWLANPassword()'>Show Password\
      <br/>\
      <hr/>\
      </p>\
      <input type='checkbox' id='sim800' name='sim800' value='1' %s/>\
      <input type='hidden' name='sim800' value='0' />\
      <label for='sim800'>I have a SIM800 module attached</label><br/>\
      <br/>\
      <input type='number' id='pin' name='pin' placeholder='PIN' size='50' value='%s'>\
      <label for='pin'>PIN</label><br/>\
      <br/>\
      <input type='text' id='apn' name='apn' placeholder='APN' size='50' value='%s'>\
      <label for='apn'>APN</label><br/>\
      <br/>\
      <input type='text' id='user' name='user' placeholder='User' size='50' value='%s'>\
      <label for='user'>User</label><br/>\
      <br/>\
      <input type='text' id='passwort' name='password' placeholder='Password' size='50' value='%s'>\
      <label for='passwort'>Password</label><br/>\
      <br/>\
      <br/>\
      <input type='text' id='MQTTbroker' name='MQTTbroker' placeholder='MQTT Broker' size='50' value='%s'>\
      <label for='MQTTbroker'>MQTT Broker</label><br/>\
      <br/>\
      <input type='text' id='MQTTport' name='MQTTport' placeholder='MQTT Port' size='50' value='%s'>\
      <label for='MQTTbroker'>MQTT Port</label><br/>\
      <br/>\
      <input type='text' id='MQTTuser' name='MQTTuser' placeholder='MQTT User' size='50' value='%s'>\
      <label for='MQTTuser'>MQTT User</label><br/>\
      <br/>\
      <input type='text' id='MQTTpassword' name='MQTTpassword' placeholder='MQTT Password' size='50' value='%s'>\
      <label for='MQTTuser'>MQTT Password</label><br/>\
      <br/><br /> <input type='submit' value='Save Configuration!' />\
      <script>\
      function showWLANPassword() {\
        var x = document.getElementById('ssidpassword');\
        if (x.type === 'password') {\
          x.type = 'text';\
        } else {\
          x.type = 'password';\
        }\
      }\
      </script>\ 
      </form>\
      ", configdoc["wlan"]["ssid"].as<char*>(),
      configdoc["wlan"]["password"].as<char*>(),
      configdoc["sim800"]["enabled"] == "true" ? "checked" : "",
      configdoc["sim800"]["pin"].as<char*>(),
      configdoc["sim800"]["apn"].as<char*>(),
      configdoc["sim800"]["user"].as<char*>(),
      configdoc["sim800"]["password"].as<char*>(),
      configdoc["sim800"]["MQTTbroker"].as<char*>(),
      configdoc["sim800"]["MQTTport"].as<char*>(),
      configdoc["sim800"]["MQTTuser"].as<char*>(),
      configdoc["sim800"]["MQTTpassword"].as<char*>());  

  if(writeConfiguration)
  {
    writeConfig();
  }
  
  char * temp = getWebsite(content);

#ifdef DEBUG
  Serial.println("temp len="+String(strlen(temp)));
#endif

  server.send(200, "text/html", temp);

  free(temp);
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
  
  ESP.reset();  
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

void reportDeviceStateInfo(String device, String deviceStateInfo)
{
  String s = "*000[D]$"+device+"$DEVICESTATEINFO:"+deviceStateInfo;
  Serial.write(s.c_str(), s.length());
  Serial.print('\n');

  // send via GSM    
  if(simEnabled)
    mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+device+"/sensor", "$DEVICESTATEINFO:"+deviceStateInfo);  
}

void mqttCallback(char* topic, byte* payload, unsigned int len) 
{
  char* p = (char*)malloc(len+1);
  memcpy(p,payload,len);
  p[len] = '\0';
  String cmd = String(p);
  free(p);  

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();

  if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd == "reset")
  {
    mqtt.publish((String(MQTT_TOPIC)+"/state").c_str(), "reset");
    delay(1);

    ESP.reset();
    delay(100);
  }
  else if(String(topic) == subscribeTopicConfig)
  {
#ifdef DEBUG   
      Serial.println("Config with parameter : "+cmd);
#endif

      StringSplitter *splitter = new StringSplitter(cmd, ' ', 4);
      int itemCount = splitter->getItemCount();

      // GATEWAY upgrade http://shrdzm.pintarweb.net/upgrade.php

      if(itemCount == 2)
      {
        setupObject.AddItem(splitter->getItemAtIndex(0), "$SC$"+splitter->getItemAtIndex(1));        
      }
      else if(itemCount == 3)
      {
        if(splitter->getItemAtIndex(0) == "GATEWAY" && splitter->getItemAtIndex(1) == "upgrade")
        {
          mqtt.publish((String(MQTT_TOPIC)+"/state").c_str(), "upgrade");          
          updateFirmware(splitter->getItemAtIndex(2));
        }
        else        
          setupObject.AddItem(splitter->getItemAtIndex(0), "$SC$"+splitter->getItemAtIndex(1), splitter->getItemAtIndex(2));        
      }
  }
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,9) == "getconfig")
  {
      getConfig();
  }
/*  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,13) == "configuration")
  {
      StringSplitter *splitter = new StringSplitter(cmd, ' ', 4);

      String configurationText = String("$configuration "+splitter->getItemAtIndex(0));
    
      swSer.write(configurationText.c_str());
      swSer.write('\n');    
  }*/
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,5) == "pair ")
  {
    simpleEspConnection.startPairing(30);
  }


  ///// ..........
}

boolean mqttConnect() 
{
  Serial.print("Connecting to ");
  Serial.print(configdoc["sim800"]["MQTTbroker"].as<char*>());


  // Or, if you want to authenticate MQTT:
  //mqtt.setSocketTimeout(30);
  //mqtt.setKeepAlive(10);
  boolean status = mqtt.connect(deviceName.c_str(), configdoc["sim800"]["MQTTuser"].as<char*>(), configdoc["sim800"]["MQTTpassword"].as<char*>());

  if (status == false) 
  {
    Serial.println(" fail");
    return false;
  }
  
  Serial.println(" success");

  mqtt.subscribe(subscribeTopicSet.c_str());
  mqtt.subscribe(subscribeTopicConfig.c_str());

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

void sendOpenESPMessages(String ad)
{  
  clientAddress = ad;
  SetupObject::SetupItem *si = setupObject.GetItem(ad);

  if(si != NULL)
  {
    String message = si->m_parameterName;

    if(si->m_parameterValue != "")
      message += ":"+ si->m_parameterValue;

#ifdef DEBUG
    Serial.printf("Send '%s' to %s\n", message.c_str(), ad.c_str());
#endif
    
    simpleEspConnection.sendMessage((char *)message.c_str(), ad);  

    setupObject.RemoveItem(si);

    if(simEnabled)
      reportDeviceStateInfo(ad, "Sent setup");
  }  
  else
  {
//    simpleEspConnection.sendMessage("$SLEEP$", ad);        
  }  
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
#ifdef DEBUG  
  Serial.println("MESSAGE:'"+String((char *)message)+"' from "+simpleEspConnection.macToStr(ad));
#endif
  
  if(String((char *)message) == "$PING$")
  {
    OnConnected(ad, simpleEspConnection.macToStr(ad));
    return;
  }

  if(String((char *)message) == "$F$") // client ask for shutdown signal
  {
    setupObject.AddItem(simpleEspConnection.macToStr(ad), "$SLEEP$");
    
    sendOpenESPMessages(simpleEspConnection.macToStr(ad));
//    simpleEspConnection.sendMessage("$SLEEP$", simpleEspConnection.macToStr(ad));        

//    Serial.println("Sent $SLEEP$ to "+simpleEspConnection.macToStr(ad));
    return;
  }

  String m = (char *)message;

  if(m.substring(0,3) == "$D$")       // Data
  {
    String s = "*000[D]$"+simpleEspConnection.macToStr(ad)+"$"+m.substring(3);
    Serial.write(s.c_str(), s.length());
    Serial.print('\n');

    // send via GSM    
    if(simEnabled)
      mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+"/sensor", m.substring(3));
  }
  else if((m.substring(0,4) == "$SC$") || // Device Configuration
          (m.substring(0,4) == "$SP$") || // Sensor Configuration
          (m.substring(0,4) == "$AP$") || // Action Configuration
          (m.substring(0,4) == "$SD$"))   // Parameter names
  {
    String mqttPrefix;
    String prefix = (m.substring(0,4) == "$SC$" || m.substring(0,4) == "$SP$") ? "C" : "P";
    if(m.substring(0,4) == "$AP$")
      prefix = "A";

    if(prefix == "C")
      mqttPrefix = "/config";
    else if(prefix == "P")
      mqttPrefix = "/param";
    else if(prefix == "A")
      mqttPrefix = "/actions";      

    
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

          if(simEnabled)
          {
            mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+mqttPrefix, 
              buffer.substring(f+1, nf));          
          }

          f = nf;
        }
        else
          run = false;
      }while(run);
  
      String s = "*000["+prefix+"]$"+simpleEspConnection.macToStr(ad)+"$"+buffer.substring(buffer.lastIndexOf('|')+1);
      Serial.write(s.c_str(), s.length());
      Serial.print('\n');

      // send via GSM    
      if(simEnabled)
      {    
        mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+mqttPrefix, 
          buffer.substring(buffer.lastIndexOf('|')+1));          
      }
    }
    else
    {
      String s = "*000["+prefix+"]$"+simpleEspConnection.macToStr(ad)+"$"+buffer;
      Serial.write(s.c_str(), s.length());
      Serial.print('\n');

      // send via GSM    
      if(simEnabled)
      {
        if(prefix == "C")
        {
          mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+"/config", 
            buffer);          
        }
        else if(prefix == "P")
        {
          mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+"/param", 
            buffer);          
        }
        else if(prefix == "A")
        {
          mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+"/actions", 
            buffer);          
        }
      }
    }        
  }
  else if(m.substring(0,3) == "$V$")  // Version
  {
    String s = "*000[V]$"+simpleEspConnection.macToStr(ad)+"$"+m.substring(3);
    Serial.write(s.c_str(), s.length());
    Serial.print('\n');

    if(simEnabled)
    {
      mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+"/version", 
        m.substring(3));              
    }
  }
  else if(m.substring(0,3) == "$X$")  // Supported devices
  {
    String s = "*000[X]$"+simpleEspConnection.macToStr(ad)+"$"+m.substring(3);
    Serial.write(s.c_str(), s.length());
    Serial.print('\n');

    if(simEnabled)
    {
      mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+"/sensors", 
        m.substring(3));                  
    }
  }
  else if(m.substring(0,3) == "$I$")  // Init due to new firmware
  {
    String s = "*000[I]$"+simpleEspConnection.macToStr(ad)+"$INIT";
    Serial.write(s.c_str(), s.length());
    Serial.print('\n');
    
    if(simEnabled)
    {
      mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/"+simpleEspConnection.macToStr(ad)+"/init", 
        "INIT");                  
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

  setupObject.AddItem(ad, "$SC$configuration");        

  // get all possible parameter
  simpleEspConnection.sendMessage("$S$", ad);    

  // send via GSM    
  if(simEnabled)
    mqttBufferObject.AddItem(String(MQTT_TOPIC)+"/paired", deviceName+"/"+ad);  
}

void OnConnected(uint8_t *ga, String ad)
{
#ifdef DEBUG
  Serial.println("EspNowConnection : Client '"+ad+"' connected! ");
#endif

  sendOpenESPMessages(ad);

/*
  clientAddress = ad;
  SetupObject::SetupItem *si = setupObject.GetItem(ad);

  if(si != NULL)
  {
    String message = si->m_parameterName;

    if(si->m_parameterValue != "")
      message += ":"+ si->m_parameterValue;

#ifdef DEBUG
    Serial.printf("Send '%s' to %s\n", message.c_str(), ad.c_str());
#endif
    
    simpleEspConnection.sendMessage((char *)message.c_str(), ad);  

    setupObject.RemoveItem(si);

    reportDeviceStateInfo(ad, "Sent setup");
  }  
  else
  {
//    simpleEspConnection.sendMessage("$SLEEP$", ad);        
  } */
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

  subscribeTopicSet = String(MQTT_TOPIC)+"/set";
  subscribeTopicConfig = String(MQTT_TOPIC)+"/config/set";

  if(!modem.isGprsConnected())
  {
    modem.init();
  }
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

  if(configdoc["sim800"]["pin"] != "" && modem.getSimStatus() != 3)
  {
    modem.simUnlock(configdoc["sim800"]["pin"]);
  }

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
  Serial.print(configdoc["sim800"]["apn"].as<char*>());

  if(!modem.isGprsConnected())
  {
    if (!modem.gprsConnect(configdoc["sim800"]["apn"].as<char*>(), configdoc["sim800"]["user"].as<char*>(), configdoc["sim800"]["password"].as<char*>())) 
    {
      Serial.println(" fail");
      return false;
    }
  }

  Serial.println(" success");

  if (modem.isGprsConnected()) 
  {
    Serial.println("GPRS connected");
  }

  localIP = modem.getLocalIP();

  mqtt.setServer(configdoc["sim800"]["MQTTbroker"].as<char*>(), atoi(configdoc["sim800"]["MQTTport"].as<char*>()));
  mqtt.setCallback(mqttCallback);

  return true;
}

bool checkSimMode()
{
  Serial.println("Will check modem...please wait.... ");
  SerialAT.begin(19200);

  if(!modem.isGprsConnected())
  {
    modem.init();
  }

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);  

  return modemInfo != "" ? true : false;
}

void setup() 
{
  bool writeConfigAndReboot = false;
  
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

#ifdef ACCESSPOINT_PIN
  ap_pin = ACCESSPOINT_PIN;
#endif  

  setDeviceName();  

  SPIFFS.begin();

  readLastVersionNumber();
  currVersion = ESP.getSketchMD5();

#ifdef DEBUG
  Serial.println("Last Version : "+lastVersionNumber+", Curr Version : "+currVersion);
#endif        
  if(!readConfig())
  {    
    writeConfig();
  }   

  if(!configdoc.containsKey("wlan"))
  {
    configdoc["wlan"]["ssid"] = "";
    configdoc["wlan"]["password"] = "";    
  }
  
  if(!configdoc.containsKey("sim800"))
  {
    configdoc["sim800"]["enabled"] = "false";
    configdoc["sim800"]["pin"] = "";
    configdoc["sim800"]["apn"] = "";
    configdoc["sim800"]["user"] = "";
    configdoc["sim800"]["password"] = "";    
    
    configdoc["sim800"]["MQTTbroker"] = "test.mosquitto.org";    
    configdoc["sim800"]["MQTTport"] = "1883";    
    configdoc["sim800"]["MQTTuser"] = "";    
    configdoc["sim800"]["MQTTpassword"] = "";    

    writeConfig();
  }

  if(ap_pin != -1)
  {
    pinMode(ap_pin, INPUT_PULLUP);  
    if(digitalRead(ap_pin) == false)
    {
  #ifdef DEBUG
      Serial.println("ACCESSPOINT Mode enabled...");
  #endif        
      accesspointmodeEnabled = true;

      APName = "SHRDZM-GW-"+deviceName;
      WiFi.hostname(APName.c_str());      

      WiFi.softAP(APName);

      server.on("/", handleRoot);
      server.on("/general", handleRoot);
      server.on("/settings", handleSettings);
      server.on("/reboot", handleReboot);  
      server.onNotFound(handleNotFound);
      server.begin();
    
      return;
    }
  }

  if(configdoc["sim800"]["enabled"] == "true")
  {
    simEnabled = checkSimMode();  
  
    // init sim
    if(simEnabled)
      initializeSIM800();
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

  bool allDataToWorkAvailable = true;

#ifdef DEBUG
  Serial.println("updateFirmware : Item count = "+String(itemCount));
#endif                    

  if(itemCount == 3)  // all data from MQTT
  {
    SSID = splitter->getItemAtIndex(0);
    password = splitter->getItemAtIndex(1);
    host = splitter->getItemAtIndex(2);
  }
  else if(itemCount == 1) // serial, now we need to check whether SSID and password is in the configuration settings
  {
    host = parameter;
    
    if(configdoc["wlan"]["ssid"] != "")
      SSID = configdoc["wlan"]["ssid"].as<char*>();
    else
      allDataToWorkAvailable = false;
      
    if(configdoc["wlan"]["password"] != "")
      password = configdoc["wlan"]["password"].as<char*>();
    else
      allDataToWorkAvailable = false;    
  }
  else
    allDataToWorkAvailable = false;    
  
  if(!allDataToWorkAvailable)
  {
    // TODO : send error message
#ifdef DEBUG
    Serial.println("updateFirmware not possible!");
#endif                    
    
    return;
  }

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

#ifdef DEBUG
    Serial.println("firmwareUpdate enabled for SSID="+SSID+", Password="+password);
#endif                    

    esp_now_deinit();
    delay(100);
  
    WiFi.mode(WIFI_STA);
  
    WiFiMulti.addAP(SSID.c_str(), password.c_str());    
}

void loop() 
{
  if(accesspointmodeEnabled)
  {
    server.handleClient();
    return;
  }
  
  simpleEspConnection.loop();

  if (simEnabled) 
  {
    if(!mqtt.connected())
    {
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
    }
    else
    {
      // send open mqtt messages
      MQTTBufferObject::BufferItem *i = mqttBufferObject.GetNextItem();
      if(i != NULL)
      {
        mqtt.publish(i->m_subject.c_str(), i->m_text.c_str());          
        mqttBufferObject.RemoveItem(i);
      }
      
      mqtt.loop();
    }    
  }
  
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
#ifdef DEBUG
            Serial.println("starting updateFirmware...");
#endif                    
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
