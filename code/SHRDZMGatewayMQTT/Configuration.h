#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "config/config.h"

/*
  {
  "configuration": {
    "wlan": {
      "SSID": "",
      "password": "",
      "MQTTHost": "test.mosquitto.org",
      "MQTTPort": "1883",
      "MQTTUser": "",
      "MQTTPassword": ""
    }
  }
}
 */

 
class Configuration
{
  public:
    Configuration();
    ~Configuration();
    bool load();
    bool store();
    bool initialize();
    String readLastVersionNumber();
    String readLastRebootInfo();
    void storeVersionNumber();
    void storeLastRebootInfo(const char *rebootinformation);

    bool containsWlanKey(char *name);
    void setWlanParameter(const char *name, const char *value);
    void setWlanParameter(JsonObject dc);
    JsonObject getWlanParameter();
    const char *getWlanParameter(const char *parameterName);


   protected:

};
#endif
