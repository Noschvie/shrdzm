#ifndef Device_SDS011_BH1750_H
#define Device_SDS011_BH1750_H

#include "DeviceBase.h"
#include <SoftwareSerial.h>

// Sensors
#include <Wire.h>
#include <hp_BH1750.h>

class Device_SDS011_BH1750 : public DeviceBase
{   
  public:
    Device_SDS011_BH1750();
    ~Device_SDS011_BH1750();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare(); 
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    hp_BH1750 BH1750;
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
