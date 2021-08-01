#include "configuration.h"

static DynamicJsonDocument g_configdoc(1524);

typedef struct struct_esp_message {
  char prefix = '.';
  char type;
  long sendTime;
  uint8_t pc; // packe count
  uint8_t p;  // package  
  uint8_t len;
  char message[200];
  uint16_t checksum;
} esp_message;

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

DynamicJsonDocument *Configuration::getConfigDocument()
{
  return &g_configdoc;
}

void Configuration::sendMessageWithChecksum(SimpleEspNowConnection *simpleEspConnection, const char *message)
{
  if(simpleEspConnection == NULL)
    return;
  
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
    em.checksum = simpleEspConnection->calculateChecksum(message);
  
    simpleEspConnection->sendMessage((uint8_t*)&em, sizeof(em));  
  }  
}
  
bool Configuration::initialize()
{
  int i = SLEEP_SECS;
  g_configdoc["interval"] = String(i);

  g_configdoc["preparetime"] = "0";

  int s = SENSORPOWERPIN;
  g_configdoc["sensorpowerpin"] = String(s);

  s = PAIRING_PIN;
  g_configdoc["pairingpin"] = String(s);

  g_configdoc["devicetype"] = "UNKNOWN";

  return true;
}

bool Configuration::store()
{
  Serial.println("Store configuration...");
  serializeJson(g_configdoc, Serial);

  Serial.println();
#ifdef LITTLEFS  
  File configFile = LittleFS.open("/shrdzm_config.json", "w");
#else
  File configFile = SPIFFS.open("/shrdzm_config.json", "w");
#endif
  Serial.println("file opened...");
  
  if (!configFile) 
  {
    Serial.println("failed to open config file for writing");
    return false;
  }

  Serial.println("serializing...");
  serializeJson(g_configdoc, configFile);
  Serial.println("serialized...");
  configFile.close();
    
  return true;
}

bool Configuration::checkCompatibility()
{
  if(g_configdoc.containsKey("devices"))
    return false;

  if(g_configdoc.containsKey("sim800"))
    return false;

  return true;
}

bool Configuration::migrateToNewConfigurationStyle()
{
  bool update = false;
  
  if(g_configdoc.containsKey("configuration"))
  {
    if(g_configdoc["configuration"].containsKey("interval"))
      g_configdoc["interval"] = g_configdoc["configuration"]["interval"];
    if(g_configdoc["configuration"].containsKey("sensorpowerpin"))
      g_configdoc["sensorpowerpin"] = g_configdoc["configuration"]["sensorpowerpin"];
    if(g_configdoc["configuration"].containsKey("pairingpin"))
      g_configdoc["pairingpin"] = g_configdoc["configuration"]["pairingpin"];
    if(g_configdoc["configuration"].containsKey("gateway"))
      g_configdoc["gateway"] = g_configdoc["configuration"]["gateway"];
    if(g_configdoc["configuration"].containsKey("devicetype"))
      g_configdoc["devicetype"] = g_configdoc["configuration"]["devicetype"];
    if(g_configdoc["configuration"].containsKey("preparetime"))
      g_configdoc["preparetime"] = g_configdoc["configuration"]["preparetime"];
    if(g_configdoc["configuration"].containsKey("processtime"))
      g_configdoc["processtime"] = g_configdoc["configuration"]["processtime"];
    if(g_configdoc["configuration"].containsKey("batterycheck"))
      g_configdoc["batterycheck"] = g_configdoc["configuration"]["batterycheck"];
    if(g_configdoc["configuration"].containsKey("device"))
      g_configdoc["device"] = g_configdoc["configuration"]["device"];

    g_configdoc.remove("configuration");

    update = true;
  }

  if(!containsWlanKey("enabled"))
  {
    setWlanParameter("enabled", "false");
    update = true;
  }
  if(!containsWlanKey("ssid"))
  {
    setWlanParameter("ssid", "");
    update = true;
  }
  if(!containsWlanKey("password"))
  {
    setWlanParameter("password", "");
    update = true;
  }
  if(!containsWlanKey("ip"))
  {
    setWlanParameter("ip", "");
    update = true;
  }
  if(!containsWlanKey("dns"))
  {
    setWlanParameter("dns", "");
    update = true;
  }
  if(!containsWlanKey("gateway"))
  {
    setWlanParameter("gateway", "");
    update = true;
  }
  if(!containsWlanKey("subnet"))
  {
    setWlanParameter("subnet", "");
    update = true;
  }
  if(!containsWlanKey("MQTTbroker"))
  {
    setWlanParameter("MQTTbroker", "test.mosquitto.org");
    update = true;
  }
  if(!containsWlanKey("MQTTport"))
  {
    setWlanParameter("MQTTport", "1883");
    update = true;
  }
  if(!containsWlanKey("MQTTuser"))
  {
    setWlanParameter("MQTTuser", "");
    update = true;
  }
  if(!containsWlanKey("MQTTpassword"))
  {
    setWlanParameter("MQTTpassword", "");
    update = true;
  }
  if(!containsCloudKey("enabled"))
  {
    setCloudParameter("enabled", "false");
    update = true;
  }
  if(!containsCloudKey("userid"))
  {
    setCloudParameter("userid", "");
    update = true;
  }
  if(!containsCloudKey("user"))
  {
    setCloudParameter("user", m_deviceName.c_str());
    update = true;
  }
  if(!containsCloudKey("password"))
  {
    setCloudParameter("password", String(ESP.getChipId()).c_str());
    update = true;
  }

  if(!g_configdoc.containsKey("sensorpowerpin"))
  {
    g_configdoc["sensorpowerpin"] = String(SENSORPOWERPIN);
    update = true;
  }
  else
  {
    if(!g_configdoc["sensorpowerpin"].is<const char*>())
    {
      g_configdoc["sensorpowerpin"] = String(SENSORPOWERPIN);
      update = true;
    }
  }
  
  if(g_configdoc["devicetype"] == "IM350")
  {
    g_configdoc["devicetype"] = "IM350/AM550";
    update = true;
  }

  return update;
}

bool Configuration::load()
{
#ifdef LITTLEFS  
  File configFile = LittleFS.open("/shrdzm_config.json", "r");
#else  
  File configFile = SPIFFS.open("/shrdzm_config.json", "r");
#endif  
  if (configFile) 
  {
    String content;
    
    for(int i=0;i<configFile.size();i++) //Read upto complete file size
    {
      content += (char)configFile.read();
    }
    
    DeserializationError error = deserializeJson(g_configdoc, content);
    if (error)
    {
      Serial.println("Error at deserializeJson");
      return false;
    }

    configFile.close();    

    serializeJson(g_configdoc, Serial);    
    Serial.println();

    // check validity
    if(g_configdoc.containsKey("interval"))
    {
      if(g_configdoc["interval"] == nullptr)
        return false;
    }
    else
      return false;
    
    if(g_configdoc.containsKey("preparetime"))
    {
      if(g_configdoc["preparetime"] == nullptr)
        return false;
    }
    else
      return false;

    if(g_configdoc.containsKey("sensorpowerpin"))
    {
      if(g_configdoc["sensorpowerpin"] == nullptr)
        return false;
    }
    else
      return false;

    if(g_configdoc.containsKey("pairingpin"))
    {
      if(g_configdoc["pairingpin"] == nullptr)
        return false;
    }
    else
      return false;

    if(g_configdoc.containsKey("devicetype"))
    {
      if(g_configdoc["devicetype"] == nullptr)
        return false;
    }
    else
      return false;

      
  }
  else
  {    
    return false;
  }
  
  return true;
}

bool Configuration::containsKey(char *name)
{
  return g_configdoc.containsKey(name);
}

bool Configuration::containsDeviceKey(char *name)
{
  return g_configdoc["device"].containsKey(name);
}

bool Configuration::containsWlanKey(char *name)
{
  return g_configdoc["wlan"].containsKey(name);
}

bool Configuration::containsCloudKey(char *name)
{
  return g_configdoc["cloud"].containsKey(name);
}

void Configuration::set(char *name, char *value)
{
  g_configdoc[name] = value;
}

void Configuration::removeAllDeviceParameter()
{
  g_configdoc.remove("device");  
  g_configdoc.createNestedObject("device");  
}

void Configuration::setDeviceParameter(const char *name, const char *value)
{
  String v(value);
  String k(name);

  v.replace( " ", "" );
  
  g_configdoc["device"][k] = v;
}

void Configuration::setWlanParameter(const char *name, const char *value)
{
  String v(value);
  String k(name);

  v.replace( " ", "" );
  
  g_configdoc["wlan"][k] = v;
}

void Configuration::setCloudParameter(const char *name, const char *value)
{
  String v(value);
  String k(name);

  v.replace( " ", "" );
  
  g_configdoc["cloud"][k] = v;
}

void Configuration::setDeviceParameter(JsonObject dc)
{
  g_configdoc["device"] = dc;
}
 
void Configuration::setWlanParameter(JsonObject dc)
{
  g_configdoc["wlan"] = dc;
}
 
void Configuration::setCloudParameter(JsonObject dc)
{
  g_configdoc["cloud"] = dc;
}
 
const char* Configuration::get(char *name)
{
  if(g_configdoc.containsKey(name))
    return g_configdoc[name].as<char*>();  
  else
    return "";
}

JsonObject Configuration::getDeviceParameter()
{
  return g_configdoc["device"];
}

JsonObject Configuration::getWlanParameter()
{
  return g_configdoc["wlan"];
}

JsonObject Configuration::getCloudParameter()
{
  return g_configdoc["cloud"];
}

const char* Configuration::getWlanParameter(const char *parameterName)
{
  return g_configdoc["wlan"][parameterName];
}

const char* Configuration::getCloudParameter(const char *parameterName)
{
  if(g_configdoc["cloud"][parameterName].isNull())
    return "";
  else
    return g_configdoc["cloud"][parameterName];
}

String Configuration::readLastVersionNumber()
{
  String lastVersionNumber = "";
  
#ifdef LITTLEFS  
  if (!(LittleFS.exists ("/version.txt") ))
#else
  if (!(SPIFFS.exists ("/version.txt") ))
#endif  
  {
    return "";
  }

#ifdef LITTLEFS  
  File file = LittleFS.open("/version.txt", "r");
#else
  File file = SPIFFS.open("/version.txt", "r");
#endif
  for(int i=0;i<file.size();i++) //Read upto complete file size
  {
    lastVersionNumber += (char)file.read();
  }

  file.close();  

  return lastVersionNumber;
}

String Configuration::readLastRebootInfo()
{
  String lastRebootInfo = "";
  
#ifdef LITTLEFS  
  if (!(LittleFS.exists ("/reboot.txt") ))
#else
  if (!(SPIFFS.exists ("/reboot.txt") ))
#endif  
  {
    return lastRebootInfo;
  }

#ifdef LITTLEFS  
  File file = LittleFS.open("/reboot.txt", "r");
#else
  File file = SPIFFS.open("/reboot.txt", "r");
#endif  

  for(int i=0;i<file.size();i++) //Read upto complete file size
  {
    lastRebootInfo += (char)file.read();
  }

  file.close();  

  return lastRebootInfo;
}

void Configuration::storeLastRebootInfo(const char *rebootinformation)
{
#ifdef LITTLEFS  
  File file = LittleFS.open("/reboot.txt", "w");
#else
  File file = SPIFFS.open("/reboot.txt", "w");
#endif  
  if (!file) 
  {
      Serial.println("Error opening reboot info file for writing");
      return;
  }  

  int bytesWritten = file.write(rebootinformation, strlen(rebootinformation));
   
  if (bytesWritten == 0) 
  {
      Serial.println("Reboot info file write failed");
  }

  file.close();
}

void Configuration::storeVersionNumber()
{
#ifdef LITTLEFS  
  File file = LittleFS.open("/version.txt", "w");
#else
  File file = SPIFFS.open("/version.txt", "w");
#endif  
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

void Configuration::sendSetup(SimpleEspNowConnection *simpleEspConnection)
{  
  String reply = "$SC$";
  
  JsonObject documentRoot = g_configdoc.as<JsonObject>();
  
  for (JsonPair kv : documentRoot) 
  {
    if(String(kv.key().c_str()) != "device" && String(kv.key().c_str()) != "wlan")
      reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";
  }

  reply.remove(reply.length()-1);

  if(!documentRoot.containsKey("devicetype"))
  {
    reply += "|devicetype: ";
  }

//  simpleEspConnection->sendMessage((char *)reply.c_str());
  sendMessageWithChecksum(simpleEspConnection,(char *)reply.c_str());                       

  Serial.println("reply 1 = "+reply);


  // send device parameter
  if(g_configdoc["device"].size() > 0)
  {
    reply = "$SP$";

    JsonObject sp = documentRoot["device"];
    for (JsonPair kv : sp) 
    {
      reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";
    }

    reply.remove(reply.length()-1);

    sendMessageWithChecksum(simpleEspConnection, (char *)reply.c_str());                       
  
    Serial.println("reply 2 = "+reply);
  }   
}

void Configuration::sendSetup(PubSubClient *mqttclient, const char *subject)
{
  JsonObject documentRoot = g_configdoc.as<JsonObject>();

  for (JsonPair kv : documentRoot) 
  {
    if(String(kv.key().c_str()) != "device")
      mqttclient->publish((String(subject)+"/config").c_str(), (kv.key().c_str()+String(":")+kv.value().as<char*>()).c_str());
  }

  if(!documentRoot.containsKey("devicetype"))
  {
      mqttclient->publish((String(subject)+"/config").c_str(), "devicetype: ");
  }

  if(g_configdoc["device"].size() > 0)
  {
    JsonObject sp = documentRoot["device"];
    for (JsonPair kv : sp) 
    {
      mqttclient->publish((String(subject)+"/config").c_str(), (kv.key().c_str()+String(":")+kv.value().as<char*>()).c_str());
    }
  }   
}

void Configuration::resetConfiguration()
{
#ifdef LITTLEFS  
  LittleFS.remove("/version.txt");
  LittleFS.remove("/reboot.txt");
  LittleFS.remove("/shrdzm_config.json");
#else
  SPIFFS.remove("/version.txt");
  SPIFFS.remove("/reboot.txt");
  SPIFFS.remove("/shrdzm_config.json");
#endif  
    
}
