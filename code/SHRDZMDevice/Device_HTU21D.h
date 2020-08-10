#ifndef Device_HTU21D_H
#define Device_HTU21D_H

#include <Wire.h>
#include "DeviceBase.h"

// Sensors
#include <HTU21D.h>

class Device_HTU21D : public DeviceBase
{   
  public:
    Device_HTU21D();
    ~Device_HTU21D();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable(){return dataAvailable;};
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    HTU21D myHTU21D;

  private:
    bool dataAvailable;
  

};

#endif
