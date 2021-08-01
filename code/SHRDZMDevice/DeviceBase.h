#ifndef DEVICEBASE_H
#define DEVICEBASE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class ConfigData
{
  public:
    ConfigData(int size);
    ~ConfigData();

    class ConfigDataItem 
    { 
      public:
        String valueI;
        ConfigDataItem(){};
        ConfigDataItem(String value);
    };  
  
    ConfigDataItem *di;
    int size;
};

class SensorData
{
  public:
    SensorData(int size);
    ~SensorData();

    class DataItem 
    { 
      public:
        String nameI;    
        String valueI;
        DataItem(){};
        DataItem(String name, String value);
    };  

    String getDataItem(String name);    
    void setDataItem(const char *name, const char *value);
  
    DataItem *di;
    int size;
};

class DeviceBase
{
  public:
    DeviceBase(){};

    JsonObject getDeviceParameter();
    virtual bool setDeviceParameter(JsonObject obj);
    JsonObject getActionParameter();
    const char * getDeviceTypeName(){return deviceTypeName.c_str();};
    void setConfigurationObject(DynamicJsonDocument *configurationObject);
    virtual bool setAction(String action){return false;};
    virtual bool setPostAction(){return false;};
    virtual void prepare(){};
    virtual bool loop(){return true;};
    virtual bool isNewDataAvailable(){return false;};
    virtual bool hasProcessEarlyEnded(){return false;};
    virtual bool initialize(){return true;};
    virtual SensorData* readParameterTypes();
    virtual SensorData* readParameter();
    virtual SensorData* readInitialSetupParameter(){return NULL;};

  protected:
    void PrintText(String text);
    String deviceTypeName;

    JsonObject deviceParameter;
    JsonObject actionParameter;

    DynamicJsonDocument *m_pConfigurationObject;
    
    StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
    StaticJsonDocument<JSON_OBJECT_SIZE(15)> docAction;
};

#endif
