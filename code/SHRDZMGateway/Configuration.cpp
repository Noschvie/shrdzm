#include "configuration.h"

static DynamicJsonDocument g_configdoc(1024);

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

void Configuration::writeDefaultConfiguration()
{
  setWlanParameter("ssid", "");
  setWlanParameter("password", "");

  setSim800Parameter("enabled", "false");
  setSim800Parameter("pin", "");
  setSim800Parameter("apn", "");
  setSim800Parameter("user", "");
  setSim800Parameter("password", "");
  setSim800Parameter("MQTTbroker", "test.mosquitto.org");
  setSim800Parameter("MQTTport", "");
  setSim800Parameter("MQTTuser", "");
  setSim800Parameter("MQTTpassword", "");

  store();
}

void Configuration::init()
{
  uint8_t pmac[6];
  WiFi.macAddress(pmac);
  m_deviceName = macToStr(pmac);

  m_deviceName.replace(":", "");
  m_deviceName.toUpperCase();

#ifdef LITTLEFS
  if(!LittleFS.begin())
  {
    DLN("First use. I need to format file system. This will take a few seconds. Please wait...");
    LittleFS.format();
    LittleFS.begin();  
    writeDefaultConfiguration();    
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
    writeDefaultConfiguration();    
  }
  else
  {
    DLN("SPIFFS accessed...");
  }
#endif
  
  load();  
}

String Configuration::macToStr(const uint8_t* mac)
{
  char mac_addr[13];
  mac_addr[12] = 0;
  
  sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

  return String(mac_addr);
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
  }
  else
  {    
    return false;
  }
  
  return true;
}

bool Configuration::store()
{
  Serial.println("Strore configuration...");
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

bool Configuration::containsKey(char *name)
{
  return g_configdoc.containsKey(name);
}

bool Configuration::containsDeviceKey(char *name)
{
  return g_configdoc["device"].containsKey(name);
}

void Configuration::set(char *name, char *value)
{
  Serial.println("Configuration set :'"+String(name)+"'-'"+String(value)+"'");  
  
  g_configdoc[name] = value;
}

bool Configuration::containsWlanKey(char *name)
{
  return g_configdoc["wlan"].containsKey(name);
}
bool Configuration::containsSim800Key(char *name)
{
  return g_configdoc["sim800"].containsKey(name);
}

void Configuration::setDeviceParameter(const char *name, const char *value)
{
  String v(value);

  v.replace( " ", "" );
  
  g_configdoc["device"][name] = v;
}

void Configuration::setDeviceParameter(JsonObject dc)
{
  g_configdoc["device"] = dc;
}
 
void Configuration::setWlanParameter(const char *name, const char *value)
{
  String v(value);

  v.replace( " ", "" );
  
  g_configdoc["wlan"][name] = v;
}

void Configuration::setWlanParameter(JsonObject dc)
{
  g_configdoc["wlan"] = dc;
}

void Configuration::setSim800Parameter(const char *name, const char *value)
{
  String v(value);

  v.replace( " ", "" );
  
  g_configdoc["sim800"][name] = v;
}

void Configuration::setSim800Parameter(JsonObject dc)
{
  g_configdoc["sim800"] = dc;
}

JsonObject Configuration::getDeviceParameter()
{
  return g_configdoc["device"];
}

JsonObject Configuration::getWlanParameter()
{
  return g_configdoc["wlan"];
}

const char* Configuration::getWlanParameter(const char *parameterName)
{
  return g_configdoc["wlan"][parameterName];
}

JsonObject Configuration::getSim800Parameter()
{
  return g_configdoc["sim800"];
}

const char* Configuration::getSim800Parameter(const char *parameterName)
{
  return g_configdoc["sim800"][parameterName];
}

void Configuration::removeAllDeviceParameter()
{
  g_configdoc.remove("device");  
  g_configdoc.createNestedObject("device");  
}
