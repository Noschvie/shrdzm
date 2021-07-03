/*
  SHRDZMGatewayMQTT

  Created 20 Jul 2020
  By Erich O. Pintar
  Modified 15 December 2021
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#define DEBUG_SHRDZM

#include "config/config.h"

Configuration configuration;

//SoftwareSerial swSer(14,12);
SoftwareSerial swSer(14,12);
ESP8266WebServer webserver;
WiFiClient espClient;
PubSubClient mqttclient(espClient);

RCSwitch mySwitch = RCSwitch();
unsigned long lastRCMillis = 0;
Ticker configurationBlinker;

#define WEBSITEBUFFER_SIZE 6000
#define MAXLINELENGTH 512

char websideBuffer[WEBSITEBUFFER_SIZE];
char menuContextBuffer[3500];
String deviceName;
String ver, nam;
String lastVersionNumber;
String currVersion;
String host;
String url;
bool configurationMode = false;
bool apConnectingOngoing = false;
String MQTT_TOPIC;
String subscribeTopicSet;
String subscribeTopicConfig;
String subscribeTopicRCSEND;
unsigned long mqttNextTry = 0;
String lastMessage = "";
const uint16_t ajaxIntervall = 2;
bool cloudConnected = false;
char cloudTokenArray[300];
String cloudID = "";
bool writeConfiguration = false;
String registerDeviceTypeBuffer = "";
time_t now;                         
tm tm; 
char cmd[MAXLINELENGTH];

void setup() 
{  
  memset(cloudTokenArray, 0, 300);
#ifdef DEBUG_SHRDZM
Serial.begin(SERIALBAUD); Serial.println();
#endif

#ifdef VERSION
  ver = String(VERSION);
#else
  ver = "0.0.0";  
#endif

#ifdef NAME
  nam = String(NAME);
#else
  nam = "SHRDZMGatewayMQTT";  
#endif

  setDeviceName();
  
#ifdef MQTT_SUBSCRIBE_TOPIC
  MQTT_TOPIC = MQTT_SUBSCRIBE_TOPIC;
#else
  MQTT_TOPIC = "SHRDZM/"+deviceName;
#endif

  subscribeTopicSet = String(MQTT_TOPIC)+"/set";
  subscribeTopicConfig = String(MQTT_TOPIC)+"/config/set";
  subscribeTopicRCSEND = String(MQTT_TOPIC)+"/RCSend";


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

  lastVersionNumber = configuration.readLastVersionNumber();  
  currVersion = ESP.getSketchMD5();

  if(strcmp(lastVersionNumber.c_str(), currVersion.c_str()) != 0)
  {
    configuration.storeVersionNumber();
  }

  if(!configuration.load())
  {
    DLN("configuration.initialize...");    
    configuration.initialize();

    DLN("New device. Will reboot to store initialisation values...");    

    configuration.store();    

    delay(100);    
    ESP.restart();          
  }

  if(configuration.containsWlanKey("NTPServer") && String(configuration.getWlanParameter("NTPServer")) != "" )
  {
    if(configuration.containsWlanKey("TZ") && String(configuration.getWlanParameter("TZ")) != "" )
    {
      configTime(configuration.getWlanParameter("TZ"), configuration.getWlanParameter("NTPServer"));
    }
  }

//  swSer.begin(SERIALBAUD, SWSERIAL_8N1, 14, 12, false);  
  swSer.begin(SERIALBAUD, SWSERIAL_8N1, 14, 12, false, 256);

  String lastRebootInfo = configuration.readLastRebootInfo();

  // set config pin
  int s = 13;
#ifdef CONFIG_PIN    
    s = CONFIG_PIN;
#endif  
  pinMode(s, INPUT_PULLUP);    

  WiFi.disconnect(true);

  if(!digitalRead(s))
  {
    configurationMode = true;
    startConfigurationAP();
  }
  else if(configuration.containsWlanKey("ssid") && String(configuration.getWlanParameter("ssid")) != "" )
  {
    startGatewayWebserver();
  }

#ifdef RESET_PIN
  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(RESET_PIN,LOW);
  delay(100);
  digitalWrite(RESET_PIN,HIGH);
#endif  

  // Enable RCSwitch
#ifdef RCSWITCHPIN
  mySwitch.enableReceive(RCSWITCHPIN);
#endif

#ifdef RCSENDPIN
  mySwitch.enableTransmit(RCSENDPIN);
#endif
    
  
}

void loop() 
{  
  if(writeConfiguration)
  {
    writeConfiguration = false;
    DLN("Store configuration...");
    configuration.store();
  }
  
  if(configurationMode)
  {
    webserver.handleClient();
    return;  
  }

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
      
      startServerListener();

      if(strcmp(configuration.getCloudParameter("enabled"),"true") == 0 && !cloudConnected)
      {
        DLN("Will start cloud connection");
        cloudConnected = cloudLogin(configuration.getCloudParameter("user"), configuration.getCloudParameter("password"));        
      }               
    } 
    else
    {
      if(millis() > 300000) // reboot every 5 minutes if no connection can be made
      {
        delay(500);
        ESP.restart();
      }
    }
  }
  else
  {
    if (WiFi.status() == WL_CONNECTED) 
    {
      webserver.handleClient();    
    }
    else
    {
      // WLAN AP disconnected
      // start reconnection process
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

  SwSerLoop();
  OTALoop();
  RCSwitchLoop();
  CloudLoop();
  
}
