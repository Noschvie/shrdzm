#ifndef DEVICEBASE_H
#define DEVICEBASE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class DeviceBase
{
  public:
    DeviceBase(){};
    DeviceBase(String deviceType);

    bool setDeviceParameter(JsonObject obj);

  protected:
    void PrintText(String text);
};

#endif
