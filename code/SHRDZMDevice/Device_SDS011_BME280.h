#ifndef Device_SDS011_BME280_H
#define Device_SDS011_BME280_H

#include "DeviceBase.h"
#include <SoftwareSerial.h>

// Sensors
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class Device_SDS011_BME280 : public DeviceBase
{   
  public:
    Device_SDS011_BME280();
    ~Device_SDS011_BME280();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare(); 
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    Adafruit_BME280 bme;  
    bool done;
    SoftwareSerial swSer;
    void gotoSleep();
    void wakeup();
    void setActiveMode();
    void setWorkingPeriod();

  private:
    bool dataAvailable;
    bool sensorAvailable;
    
  

};

#endif
