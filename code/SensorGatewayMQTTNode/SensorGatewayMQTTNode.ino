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

SoftwareSerial swSer(14,12);
WiFiServer server(80);
configData_t cfg;
int cfgStart= 0;
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
//  swSer.begin(SERIALBAUD, SWSERIAL_8N1, 14, 12, false);  
  swSer.begin(SERIALBAUD);  
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
  else
  {
#ifdef DEBUG  
    Serial.println();
    Serial.println("no valid configuration found. Will use default and start AP");
#endif  
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
  APName.toUpperCase();
  WiFi.hostname(APName.c_str());
  
  WiFiManagerParameter custom_text0("<p><strong> MQTT Settings</p> </strong>");
  wifiManager.addParameter(&custom_text0);
  
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
    cfg.valid = 1;
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
    //  Serial.println("Command : "+cmd);
#endif        

      StringSplitter *splitter = new StringSplitter(cmd, '$', 3);
      int itemCount = splitter->getItemCount();
    
      for(int i = 0; i < itemCount; i++)
      {
        String item = splitter->getItemAtIndex(i);
        
        Serial.println(String(i)+":"+item);
      }    
  
      delete splitter;       
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
  }
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
