#ifndef Device_BMP280_H
#define Device_BMP280_H

#include <Wire.h>
#include "DeviceBase.h"

// Sensors
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

class Device_BMP280 : public DeviceBase
{   
  public:
    Device_BMP280();
    ~Device_BMP280();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    Adafruit_BMP280 bmp;
    Adafruit_Sensor *bmp_temp;
    Adafruit_Sensor *bmp_pressure;

  private:
    bool dataAvailable;
    bool sensorAvailable;


};

#endif
