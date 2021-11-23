#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "config/config.h"
#include "SimpleEspNowConnection.h"

#define MAX_ESP_BUFFERSIZE 500

class Configuration
{
  public:
    Configuration();
    ~Configuration();
    char sendESPBuffer[MAX_ESP_BUFFERSIZE];

    void setDeviceName(const char* devicename)
    {
      m_deviceName = devicename;
    };
    bool load();
    bool store();
    bool initialize();
    bool containsKey(char *name);
    bool containsDeviceKey(char *name);
    bool containsWlanKey(char *name);
    bool containsCloudKey(char *name);
    bool checkCompatibility();
    JsonObject getDeviceParameter();
    JsonObject getWlanParameter();
    JsonObject getCloudParameter();
    DynamicJsonDocument *getConfigDocument();
    const char *getWlanParameter(const char *parameterName);
    const char *getCloudParameter(const char *parameterName);
    void set(char *name, char *value);
    void setDeviceParameter(const char *name, const char *value);
    void removeAllDeviceParameter();
    void setWlanParameter(const char *name, const char *value);
    void setCloudParameter(const char *name, const char *value);
    void setDeviceParameter(JsonObject dc);    
    void setWlanParameter(JsonObject dc);
    void setCloudParameter(JsonObject dc);
    bool migrateToNewConfigurationStyle();
    const char *get(char *name);
    String readLastVersionNumber();
    String readLastRebootInfo();
    void storeVersionNumber();
    void storeLastRebootInfo(const char *rebootinformation);
    void sendSetup(SimpleEspNowConnection *simpleEspConnection);
    void sendSetup(PubSubClient *mqttclient, const char *subject);
    void resetConfiguration();
   protected:
    void sendMessageWithChecksum(SimpleEspNowConnection *simpleEspConnection, const char *message);
    String m_deviceName;

};

#endif
