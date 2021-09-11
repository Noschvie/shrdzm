#include "configuration.h"

static DynamicJsonDocument g_configdoc(1024);

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

bool Configuration::initialize()
{
  g_configdoc.clear();
  
  g_configdoc["configuration"]["wlan"]["ssid"] = "";
  g_configdoc["configuration"]["wlan"]["password"] = "";
  g_configdoc["configuration"]["wlan"]["MQTTenabled"] = "true";
  g_configdoc["configuration"]["wlan"]["MQTTbroker"] = "test.mosquitto.org";
  g_configdoc["configuration"]["wlan"]["MQTTport"] = "1883";
  g_configdoc["configuration"]["wlan"]["MQTTuser"] = "";
  g_configdoc["configuration"]["wlan"]["MQTTpassword"] = "";
  g_configdoc["configuration"]["wlan"]["MQTTpassword"] = "";

  return true;
}

bool Configuration::store()
{
  serializeJson(g_configdoc, Serial);
  Serial.println();

#ifdef LITTLEFS  
  File configFile = LittleFS.open("/shrdzm_config.json", "w");
#else
  File configFile = SPIFFS.open("/shrdzm_config.json", "w");
#endif
  
  if (!configFile) 
  {
    return false;
  }

  serializeJson(g_configdoc, configFile);
  configFile.close();
    
  return true;
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
      return false;
    }

    configFile.close();    

    serializeJson(g_configdoc, Serial);    
    Serial.println();
    
    if(!g_configdoc["configuration"]["wlan"].containsKey("ssid"))
    {
#ifdef LITTLEFS  
  LittleFS.remove("/shrdzm_config.json"); 
#else  
  SPIFFS.remove("/shrdzm_config.json"); 
#endif  
      return false;
    }

    if(!g_configdoc["configuration"]["wlan"].containsKey("NTPServer"))
    {
      setWlanParameter("NTPServer", String(NTP_SERVER).c_str());
      store();
    }
    if(!g_configdoc["configuration"]["wlan"].containsKey("TZ"))
    {
      setWlanParameter("TZ", String(TZ).c_str());
      store();
    }
    if(!g_configdoc["configuration"]["wlan"].containsKey("MQTTenabled"))
    {
      setWlanParameter("MQTTenabled", "true");
      store();
    }      
    if(!g_configdoc["configuration"]["wlan"].containsKey("ip"))
    {
      setWlanParameter("ip", "");
      store();
    }
    if(!g_configdoc["configuration"]["wlan"].containsKey("dns"))
    {
      setWlanParameter("dns", "");
      store();
    }
    if(!g_configdoc["configuration"]["wlan"].containsKey("gateway"))
    {
      setWlanParameter("gateway", "");
      store();
    }
    if(!g_configdoc["configuration"]["wlan"].containsKey("subnet"))
    {
      setWlanParameter("subnet", "");
      store();
    }      
  }
  else
  {    
    return false;
  }
  
  return true;
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

void Configuration::storeVersionNumber()
{
#ifdef LITTLEFS  
  File file = LittleFS.open("/version.txt", "w");
#else
  File file = SPIFFS.open("/version.txt", "w");
#endif  
  if (!file) 
  {
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
  }

  file.close();
}

bool Configuration::containsWlanKey(char *name)
{
  return g_configdoc["configuration"]["wlan"].containsKey(name);
}

bool Configuration::containsCloudKey(char *name)
{
  return g_configdoc["configuration"]["cloud"].containsKey(name);
}

void Configuration::setWlanParameter(const char *name, const char *value)
{
  String v(value);

  v.replace( " ", "" );
  
  g_configdoc["configuration"]["wlan"][name] = v;
}

void Configuration::setWlanParameter(JsonObject dc)
{
  g_configdoc["configuration"]["wlan"] = dc;
}

void Configuration::setCloudParameter(const char *name, const char *value)
{
  String v(value);

  v.replace( " ", "" );
  
  g_configdoc["configuration"]["cloud"][name] = v;
}

JsonObject Configuration::getWlanParameter()
{
  return g_configdoc["configuration"]["wlan"];
}

JsonObject Configuration::getCloudParameter()
{
  return g_configdoc["configuration"]["cloud"];
}

const char* Configuration::getWlanParameter(const char *parameterName)
{
  if(g_configdoc["configuration"]["wlan"][parameterName].isNull())
    return "";
  else
    return g_configdoc["configuration"]["wlan"][parameterName];
}

const char* Configuration::getCloudParameter(const char *parameterName)
{
  if(g_configdoc["configuration"]["cloud"][parameterName].isNull())
    return "";
  else
    return g_configdoc["configuration"]["cloud"][parameterName];
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
      return;
  }  

  int bytesWritten = file.write(rebootinformation, strlen(rebootinformation));
   
  if (bytesWritten == 0) 
  {
  }

  file.close();
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
