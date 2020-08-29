#ifndef Device_SDS011_BMP280_H
#define Device_SDS011_BMP280_H

#include "DeviceBase.h"
#include <SoftwareSerial.h>

#include <Wire.h>
#include "DeviceBase.h"

// Sensors
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>


class Device_SDS011_BMP280 : public DeviceBase
{   
  public:
    Device_SDS011_BMP280();
    ~Device_SDS011_BMP280();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare(); 
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    Adafruit_BMP280 bmp;
    Adafruit_Sensor *bmp_temp;
    Adafruit_Sensor *bmp_pressure;

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
