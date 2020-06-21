#ifndef Device_BME280_H
#define Device_BME280_H

#include <Wire.h>
#include "DeviceBase.h"

// Sensors
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class Device_BME280 : public DeviceBase
{   
  public:
    Device_BME280();
    ~Device_BME280();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    Adafruit_BME280 bme;

  private:
  

};

#endif
