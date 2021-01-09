#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "config/config.h"

class Configuration
{
  public:
    Configuration();
    ~Configuration();
    void init();
    bool load();
    bool store();    
    bool containsKey(char *name);
    bool containsDeviceKey(char *name); 
    bool containsWlanKey(char *name);
    bool containsSim800Key(char *name);    
    const char *get(char *name);   
    void set(char *name, char *value);     
    void setDeviceParameter(const char *name, const char *value);
    void setDeviceParameter(JsonObject dc);    
    void setWlanParameter(const char *name, const char *value);
    void setWlanParameter(JsonObject dc);
    void setSim800Parameter(const char *name, const char *value);
    void setSim800Parameter(JsonObject dc);
    JsonObject getDeviceParameter();
    JsonObject getWlanParameter();
    const char *getWlanParameter(const char *parameterName);
    JsonObject getSim800Parameter();
    const char *getSim800Parameter(const char *parameterName);
    void removeAllDeviceParameter();
       
    String macToStr(const uint8_t* mac);
    
    String m_deviceName;

  protected:
    void writeDefaultConfiguration();
};

#endif
