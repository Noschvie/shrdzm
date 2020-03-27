#include <FS.H>
#include "config/config.h"

#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "StringSplitter.h"
#include <MQTT.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <PubSubClient.h>
#include <EEPROM.h>

String MQTT_TOPIC;
String subcribeTopicSet;
String subcribeTopicConfig;
String nodeName;
String deviceName;
DynamicJsonDocument configdoc(1024);
JsonObject configuration  = configdoc.createNestedObject("configuration");
JsonObject web_configuration  = configuration.createNestedObject("web");

#ifdef RCSWITCH_SUPPORT
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
unsigned long lastRCMillis = 0;
#ifdef RCSENDPIN  
  String subcribeTopicRCSEND;
#endif
#endif


typedef struct 
{
  int valid;                        // 0=no configuration, 1=valid configuration
  char SSID[31];                    // SSID of WiFi
  char password[61];                // Password of WiFi
  char MQTTHost[61];                // Ip address or hostname of MQTT broker
  char MQTTPort[11];                // Port of MQTT broker
  char MQTTUser[31]; 
  char MQTTPassword[41];          
} configData_t;

SoftwareSerial swSer(14,12);
//WiFiServer server(80);
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

configData_t cfg;
int cfgStart= 0;
bool shouldSaveConfig = false;
bool rcSwitchAvailableBuffer = false;

const char* MQTTHost = "127.0.0.1";
const char* MQTTPort = "1883";
const char* MQTTUser = NULL;
const char* MQTTPassword = NULL;

void saveConfigCallback () 
{
#ifdef DEBUG
  Serial.println("Should save config");
#endif  
  shouldSaveConfig = true;
}

bool readConfig()
{
    if (SPIFFS.exists("/shrdzmMQTT_config.json")) 
    {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/shrdzmMQTT_config.json", "r");
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
      Serial.println("shrdzmMQTT_config.json does not exist");
#endif
      return false;
    }
}

bool writeConfig()
{
    File configFile = SPIFFS.open("/shrdzmMQTT_config.json", "w");
    if (!configFile) 
    {
#ifdef DEBUG
      Serial.println("failed to open config file for writing");
#endif
      return false;
    }

    serializeJson(configdoc, configFile);
    configFile.close();
}

void eraseConfig() 
{
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(sizeof(configData_t));
  for (int i = cfgStart ; i < sizeof(cfg) ; i++) 
  {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();

#ifdef DEBUG
  Serial.println("config erased!");
#endif  
}

void saveConfig() 
{
  // Save configuration from RAM into EEPROM
  EEPROM.begin(sizeof(configData_t));
  EEPROM.put( cfgStart, cfg );
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure

#ifdef DEBUG  
  Serial.println("new configuration stored");
  
  Serial.println("MQTT Server :"+String(cfg.MQTTHost));
#endif  
  
}

void loadConfig() 
{
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(sizeof(configData_t));
  EEPROM.get( cfgStart, cfg );
  EEPROM.end();
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

void handleRoot() 
{
  char temp[1800];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

#ifdef DEBUG
  Serial.println("Handle Root");
#endif

  snprintf(temp, 1800,
  
"<!DOCTYPE html>\
<html>\
<head>\
<style>\
\
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
</style>\
<title>SHRDZM - %s</title>\
</head>\
<body>\
\
<ul>\
  <li>\
    <a class='active' href='#home'>SHRDZM<br/>\
      <font size='2'>%s</font>\
    </a></li>\
  <li><a href='./general'>General</a></li>\
  <li><a href='./devices'>Devices</a></li>\
  <li><a href='./about'>Settings</a></li>\
  <li><a href='./about'>About</a></li>\
  <li><a href='./reboot'>Reboot</a></li>\
</ul>\
\
</body>\
</html>\
  ", deviceName.c_str(), deviceName.c_str());

/*  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>SHRDZM</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from SHRDZM!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
          */
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

void setup() 
{  
#ifdef DEBUG  
  Serial.begin(9600);
//  Serial.print("SSID:"+String(cfg.SSID));
#endif  

  setDeviceName();
  
  WiFiManager wifiManager;
  SPIFFS.begin();

//  eraseConfig();
//  wifiManager.resetSettings();


#ifdef RCSWITCH_SUPPORT
  mySwitch.enableReceive(RCSWITCHPIN);
#ifdef DEBUG
  Serial.println("RCSWITCH_SUPPORT on Port "+String(RCSWITCHPIN));
#endif

#ifdef RCSENDPIN
  mySwitch.enableTransmit(RCSENDPIN);
#ifdef DEBUG
  Serial.println("RCSENDPIN on Port "+String(RCSENDPIN));
#endif
#endif
#endif


  if(!readConfig())
  {
    SPIFFS.format();

    web_configuration["SSID"] = "";
    web_configuration["password"] = "";
    web_configuration["MQTTHost"] = "test.mosquitto.org";
    web_configuration["MQTTPort"] = "1883";
    web_configuration["MQTTUser"] = "";
    web_configuration["MQTTPassword"] = "";

    writeConfig();
  }

// !!  
  loadConfig();  
  

#ifdef SERIALBAUD
  swSer.begin(SERIALBAUD, SWSERIAL_8N1, 14, 12, false);  
#else
  swSer.begin(9600, SWSERIAL_8N1, 14, 12, false);
#endif

#ifdef RESET_PIN
  pinMode(RESET_PIN, OUTPUT);
#endif

  MQTTHost = web_configuration["MQTTHost"];
  MQTTPort = web_configuration["MQTTPort"];

  if(web_configuration["MQTTUser"] == "")
  {
    MQTTUser = NULL;
  }
  else
  {
    MQTTUser = web_configuration["MQTTUser"];
  }

  if(web_configuration["MQTTPassword"] == "")
  {
    MQTTPassword = NULL;
  }
  else
  {
    MQTTPassword = web_configuration["MQTTPassword"];    
  }

  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", web_configuration["MQTTHost"], 60);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", web_configuration["MQTTPort"], 10);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", web_configuration["MQTTUser"], 30);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", web_configuration["MQTTPassword"], 40);
  
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  String APName = "SHRDZM-"+deviceName;
  WiFi.hostname(APName.c_str());

  
  WiFiManagerParameter custom_text0("<p><strong> MQTT Settings</p> </strong>");
  wifiManager.addParameter(&custom_text0);
  
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  
  wifiManager.autoConnect(APName.c_str());

  web_configuration["MQTTHost"] = custom_mqtt_server.getValue();
  web_configuration["MQTTPort"] = custom_mqtt_port.getValue();
  web_configuration["MQTTUser"] = custom_mqtt_user.getValue();
  web_configuration["MQTTPassword"] = custom_mqtt_pass.getValue();


  // !!!!!!!!!!
  strcpy(cfg.MQTTHost, custom_mqtt_server.getValue());
  strcpy(cfg.MQTTPort, custom_mqtt_port.getValue());
  strcpy(cfg.MQTTUser, custom_mqtt_user.getValue());
  strcpy(cfg.MQTTPassword, custom_mqtt_pass.getValue());

  if (shouldSaveConfig)
  {
    cfg.valid = 1;

    writeConfig();

    // !!!!!
    saveConfig();

    delay(100);
    ESP.restart();      
  }

#ifdef MQTT_SUBSCRIBE_TOPIC
  MQTT_TOPIC = MQTT_SUBSCRIBE_TOPIC;
#else
  MQTT_TOPIC = "SHRDZM/"+deviceName;
#endif

  nodeName = MQTT_TOPIC;

  subcribeTopicSet = String(MQTT_TOPIC)+"/set";
  subcribeTopicConfig = String(MQTT_TOPIC)+"/config/set";

#ifdef RCSENDPIN  
  subcribeTopicRCSEND = String(MQTT_TOPIC)+"/RCSend";
#endif

#ifdef DEBUG
  Serial.println("MQTTHost : "+String(MQTTHost));
  Serial.println("MQTTPort : "+String(MQTTPort));
  Serial.println("MQTTUser : "+String(MQTTUser));
  Serial.println("MQTTPassword : "+String(MQTTPassword));
  Serial.println("MQTT_TOPIC : "+String(MQTT_TOPIC));
  Serial.println("MQTT_TOPIC_SUBSCRIBE Set : "+String(subcribeTopicSet));
  Serial.println("MQTT_TOPIC_SUBSCRIBE Config : "+String(subcribeTopicConfig));

#ifdef RCSENDPIN  
  Serial.println("MQTT_TOPIC_SUBSCRIBE RCSend : "+String(subcribeTopicRCSEND));
#endif
#endif

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();

  client.setServer(MQTTHost, String(MQTTPort).toInt());
  client.setCallback(callback);

#ifdef RESET_PIN
  digitalWrite(RESET_PIN,LOW);
  delay(100);
  digitalWrite(RESET_PIN,HIGH);
#endif  
}

void sendSensorData(String data)
{
  StringSplitter *splitter = new StringSplitter(data, '$', 3);
  int itemCount = splitter->getItemCount();
  String subject = "SHRDZM/";

  if(itemCount == 3)
  {
    if(splitter->getItemAtIndex(2).indexOf(':') > 0)
    {
      String v = splitter->getItemAtIndex(2).substring(splitter->getItemAtIndex(2).indexOf(':')+1);
      String t = splitter->getItemAtIndex(2).substring(0, splitter->getItemAtIndex(2).indexOf(':'));

      if(splitter->getItemAtIndex(0) == "[D]")
      {
        client.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/sensor").c_str(), 
          splitter->getItemAtIndex(2).c_str());
      }
      else if(splitter->getItemAtIndex(0) == "[I]")  // report status
      {
        client.publish((String(MQTT_TOPIC)+"/state").c_str(), String(t+" "+v).c_str());
      }
      else if(splitter->getItemAtIndex(0) == "[P]") // [P]$500291D60619$paired:OK
      {        
        client.publish((String(MQTT_TOPIC)+"/paired").c_str(), 
          String(deviceName+"/"+splitter->getItemAtIndex(1)).c_str());
      }      
      else if(splitter->getItemAtIndex(0) == "[C]") // [C]$F4CFA2D4009B$interval:30
      {
        client.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/config").c_str(), 
          splitter->getItemAtIndex(2).c_str());
      }
    }
  }

  delete splitter;         
}

void sendRCData(String data)
{
#ifdef DEBUG   
    Serial.println("Publish : topic : "+(String(MQTT_TOPIC)+"/RCData")+" Value: "+data);
#endif

  client.publish((String(MQTT_TOPIC)+"/RCData").c_str(), data.c_str());
}

void loop() 
{
  server.handleClient();
  
#ifdef RCSWITCH_SUPPORT
  if (mySwitch.available()) 
  {
    if(millis() - lastRCMillis > 1000)
    {
      unsigned long RCData = mySwitch.getReceivedValue();
      sendRCData(String(RCData));

  #ifdef DEBUG
      Serial.println("Incoming RCData : "+String(RCData));
  #endif    
    }

    lastRCMillis = millis();
    mySwitch.resetAvailable();
  }
#endif

  if (!client.connected()) 
  {
    if (client.connect(nodeName.c_str(),
          MQTTUser, 
          MQTTPassword
          )) 
    {
      client.publish((String(MQTT_TOPIC)+"/state").c_str(), "up");

      client.subscribe(subcribeTopicSet.c_str());
      client.subscribe(subcribeTopicConfig.c_str());
      

//      Serial.println(subcribeTopicRCSEND);      
#ifdef RCSWITCH_SUPPORT            
      client.subscribe(subcribeTopicRCSEND.c_str());
#endif
      
      delay(1);
    }
    else
    {
#ifdef DEBUG
      Serial.print("connecting to mqtt broker failed, rc: ");
      Serial.println(client.state());
#endif      
      delay(1000);
    }
  }

  while (swSer.available()) 
  {
    if ( swSer.read() == '*' ) 
    {
      while ( ! swSer.available() ) 
      { 
        delay(1); 
      }
      String cmd = readSerialSS();

#ifdef DEBUG
      Serial.println("Command : "+cmd);
#endif        
      sendSensorData(cmd);      
    }
  }

  if (Serial.available() > 0)
  {
    String cmd = readSerialHW();

    if(cmd == "$reset")
    {
#ifdef DEBUG
      Serial.println("Reset..");
#endif        
      ESP.restart();      
    }
    else if(cmd == "$pair")
    {
      swSer.write("$pair");
      swSer.write('\n');    
    }
    else if(cmd.substring(0,5) == "$set ")
    {
      swSer.write(cmd.c_str());
      swSer.write('\n');    
    }    
    else if(cmd == "$init")
    {
      WiFiManager wifiManager;
      
      eraseConfig();
      wifiManager.resetSettings();
      delay(100);

      ESP.restart();            
    }    
  }
  
  client.loop();  
}

String readSerialSS()
{
  char cc[4];
  while (swSer.available() < 3) { delay(1); }

  cc[0] = swSer.read();
  cc[1] = swSer.read();
  cc[2] = swSer.read();
  cc[3] = 0;

  int len =  String(cc).toInt();
  char buffer[len+1];
  buffer[len] = 0;
  
  while (swSer.available() < 1) { delay(1); }

  while (swSer.available() < len) { delay(1); }
    
  swSer.readBytes(buffer, len);  
  
  return String(buffer);
}

String readSerialHW()
{
  String cmd = "";
  byte inByte = 0;
  int counter = 0;
  bool finished = false;
  
  while (!finished)
  {
    while (Serial.available() == 0 && counter < 50) 
    { 
      delay(1); 
      counter++;
    }
    if(counter == 50)
    {
      finished = true;
    }
    else
    {
      counter = 0;
      inByte = Serial.read();

      yield();
      
      if (inByte == '\n')
      {
        finished = true;
      }
      else
      {
        cmd += (char)inByte;
      }
    }    
  }  

  delay(100);
  
  return cmd;  
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  char* p = (char*)malloc(length+1);
  memcpy(p,payload,length);
  p[length] = '\0';
  String cmd = String(p);
  free(p);

#ifdef DEBUG   
  Serial.println("MQTT topic : "+String(topic));
  Serial.println("MQTT command : "+cmd);
#endif

#ifdef RCSENDPIN
  if(String(topic) == (String(MQTT_TOPIC)+"/RCSend"))
  {
    StringSplitter *splitter = new StringSplitter(cmd, ',', 3);
    int itemCount = splitter->getItemCount();
    
    if(itemCount == 3) // on/off housecode switchcode
    {
      bool onoff = false;
      String onoffText = splitter->getItemAtIndex(0);
      
      if(onoffText == "1"  || onoffText == "on" || onoffText == "ON" || onoffText == "ein" || onoffText == "EIN")
      {
        onoff = true;
      }

      if(onoff)
      {
        mySwitch.switchOn(splitter->getItemAtIndex(1).c_str(), splitter->getItemAtIndex(2).c_str());
      }
      else
      {
        mySwitch.switchOff(splitter->getItemAtIndex(1).c_str(), splitter->getItemAtIndex(2).c_str());
      }      
    }
    else
    {
      mySwitch.send(cmd.toInt(), 24);
#ifdef DEBUG   
      Serial.println("RCSend : "+cmd.toInt());
#endif
    }
  }
#endif
  
  if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd == "reset")
  {
    client.publish((String(MQTT_TOPIC)+"/state").c_str(), "reset");
    delay(1);

    ESP.reset();
  }
  else if(String(topic) == subcribeTopicConfig)
  {
#ifdef DEBUG   
      Serial.println("Config with parameter : "+cmd);
#endif
      swSer.write(String("$set "+cmd).c_str());
      swSer.write('\n'); 
  }
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,5) == "pair ")
  {
      swSer.write("$pair");
      swSer.write('\n');    
  }
}
