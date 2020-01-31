#include "config/config.h"

#include <SoftwareSerial.h>
#include "StringSplitter.h"
#include <MQTT.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <EEPROM.h>

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

SoftwareSerial swSer;
WiFiServer server(80);
configData_t cfg;
int cfgStart= 512;
bool shouldSaveConfig = false;

const char* MQTTHost = "127.0.0.1";
const char* MQTTPort = "1883";
const char* MQTTUser = "";
const char* MQTTPassword = "";

void saveConfigCallback () 
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
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
}

void saveConfig() 
{
  // Save configuration from RAM into EEPROM
  EEPROM.begin(sizeof(configData_t));
  EEPROM.put( cfgStart, cfg );
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
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
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void setup() 
{  
  WiFiManager wifiManager;

//  eraseConfig();
//  wifiManager.resetSettings();
  
  loadConfig();  
#ifdef DEBUG  
  Serial.begin(9600);

  Serial.print("SSID:"+String(cfg.SSID));
#endif  

#ifdef SERIALBAUD
  swSer.begin(SERIALBAUD, SWSERIAL_8N1, 14, 12, false);  
#else
  swSer.begin(9600, SWSERIAL_8N1, 14, 12, false);
#endif

  if(cfg.valid == 1)
  {
    MQTTHost = cfg.MQTTHost;
    MQTTPort = cfg.MQTTPort;
    MQTTUser = cfg.MQTTUser;
    MQTTPassword = cfg.MQTTPassword;
  }
 
  
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", MQTTHost, 60);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", MQTTPort, 10);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", MQTTUser, 30);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", MQTTPassword, 40);
  
  
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  uint8_t xmac[6];
  WiFi.macAddress(xmac);

  String APName = "SHRDZM-"+macToStr(xmac);
  APName.replace(":", "");

  wifiManager.resetSettings();

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  
  wifiManager.autoConnect(APName.c_str());

  strcpy(cfg.MQTTHost, custom_mqtt_server.getValue());
  strcpy(cfg.MQTTPort, custom_mqtt_port.getValue());
  strcpy(cfg.MQTTUser, custom_mqtt_user.getValue());
  strcpy(cfg.MQTTPassword, custom_mqtt_pass.getValue());

  if (shouldSaveConfig)
  {
    saveConfig();
  }

  server.begin();    
}

void loop() 
{
  WiFiClient client = server.available();

  if (client) 
  {   
    Serial.println("New Client.");
  }

  if (swSer.available() > 0)
  {
    String cmd = readSerial();

#ifdef DEBUG
    Serial.println("Command : "+cmd);
#endif        

    StringSplitter *splitter = new StringSplitter(cmd, '$', 5);
    int itemCount = splitter->getItemCount();
  

    for(int i = 0; i < itemCount; i++)
    {
      String item = splitter->getItemAtIndex(i);
      
      Serial.println(String(i)+":"+item);
    }    

    delete splitter;
  }    

  
  
}

String readSerial() 
{
  String cmd = "";
  byte inByte = 0;
  int counter = 0;
  bool finished = false;
  
  while (!finished)
  {
    while (swSer.available() > 0)     
    {
      counter = 0;
      inByte = swSer.read();
      
      if (inByte == '\n')
      {
        finished = true;
      }
      else
      {
        cmd += (char)inByte;
      }
    }    
    yield();
  }  

  return cmd;
}
