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
  int i = SLEEP_SECS;
  g_configdoc["interval"] = String(i);

  g_configdoc["preparetime"] = "0";

  int s = SENSORPOWERPIN;
  g_configdoc["sensorpowerpin"] = String(s);

  s = PAIRING_PIN;
  g_configdoc["pairingpin"] = String(s).c_str();

  g_configdoc["devicetype"] = "UNKNOWN";

  return true;
}

bool Configuration::store()
{
  File configFile = SPIFFS.open("/shrdzm_config.json", "w");
  if (!configFile) 
  {
    Serial.println("failed to open config file for writing");
    return false;
  }

  serializeJson(g_configdoc, configFile);
  configFile.close();
    
  return true;
}

bool Configuration::migrateToNewConfigurationStyle()
{
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

    return true;
  }

  return false;
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

void Configuration::set(char *name, char *value)
{
  g_configdoc[name] = value;
}

void Configuration::setDeviceParameter(char *name, char *value)
{
  g_configdoc["device"][name] = value;
}


void Configuration::setDeviceParameter(JsonObject dc)
{
  g_configdoc["device"] = dc;
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
