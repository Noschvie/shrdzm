#ifndef DEVICEBASE_H
#define DEVICEBASE_H

#include <Arduino.h>
#include <ArduinoJson.h>

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
  
    DataItem *di;
    int size;
};

class DeviceBase
{
  public:
    DeviceBase(){};
    DeviceBase(String deviceType);

    virtual bool setDeviceParameter(JsonObject obj);
    virtual SensorData* readParameterTypes();
    virtual SensorData* readParameter();

  protected:
    void PrintText(String text);

    JsonObject deviceParameter;
};

#endif
