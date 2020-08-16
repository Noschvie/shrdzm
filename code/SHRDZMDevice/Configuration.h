#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "config/config.h"
#include "SimpleEspNowConnection.h"

class Configuration
{
  public:
    Configuration();
    ~Configuration();

    bool load();
    bool store();
    bool initialize();
    bool containsKey(char *name);
    bool containsDeviceKey(char *name);
    JsonObject getDeviceParameter();
    void set(char *name, char *value);
    void setDeviceParameter(char *name, char *value);
    void setDeviceParameter(JsonObject dc);
    bool migrateToNewConfigurationStyle();
    const char *get(char *name);
    String readLastVersionNumber();
    void storeVersionNumber();
    void sendSetup(SimpleEspNowConnection *simpleEspConnection);
  protected:

};

#endif
