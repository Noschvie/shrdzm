#include "configuration.h"

static DynamicJsonDocument g_configdoc(1024);

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
  Serial.println("Strore configuration...");
  serializeJson(g_configdoc, Serial);

  Serial.println();
  File configFile = SPIFFS.open("/shrdzm_config.json", "w");
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

  return update;
}

bool Configuration::load()
{
  File configFile = SPIFFS.open("/shrdzm_config.json", "r");
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

void Configuration::set(const char *name, const char *value)
{
  Serial.println("Configuration set :'"+String(name)+"'-'"+String(value)+"'");  
  
  g_configdoc[name] = value;
}

void Configuration::setDeviceParameter(const char *name, const char *value)
{
  String v(value);

  v.replace( " ", "" );
  
  g_configdoc["device"][name] = v;
}

void Configuration::setWlanParameter(const char *name, const char *value)
{
  String v(value);

  v.replace( " ", "" );
  
  g_configdoc["wlan"][name] = v;
}

void Configuration::setDeviceParameter(JsonObject dc)
{
  g_configdoc["device"] = dc;
}
 
void Configuration::setWlanParameter(JsonObject dc)
{
  g_configdoc["wlan"] = dc;
}
 
const char* Configuration::get(char *name)
{
  if(g_configdoc.containsKey(name))
    return g_configdoc[name];  
  else
    return NULL;
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

String Configuration::readLastVersionNumber()
{
  String lastVersionNumber = "";
  
  if (!(SPIFFS.exists ("/version.txt") ))
  {
    return "";
  }

  File file = SPIFFS.open("/version.txt", "r");

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
  
  if (!(SPIFFS.exists ("/reboot.txt") ))
  {
    return "";
  }

  File file = SPIFFS.open("/reboot.txt", "r");

  for(int i=0;i<file.size();i++) //Read upto complete file size
  {
    lastRebootInfo += (char)file.read();
  }

  file.close();  

  return lastRebootInfo;
}

void Configuration::storeLastRebootInfo(const char *rebootinformation)
{
  File file = SPIFFS.open("/reboot.txt", "w");
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
  File file = SPIFFS.open("/version.txt", "w");
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
    if(String(kv.key().c_str()) != "device")
      reply += kv.key().c_str()+String(":")+kv.value().as<char*>()+"|";
  }

  reply.remove(reply.length()-1);

  if(!documentRoot.containsKey("devicetype"))
  {
    reply += "|devicetype: ";
  }

  simpleEspConnection->sendMessage((char *)reply.c_str());

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

    simpleEspConnection->sendMessage((char *)reply.c_str());
  }   
}

void Configuration::sendSetup(PubSubClient *mqttclient, const char *subject)
{
//  String reply;
  
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
