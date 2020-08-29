#ifndef Device_BH1750_H
#define Device_BH1750_H

#include <Wire.h>
#include "DeviceBase.h"

// Sensors
#include <hp_BH1750.h>

class Device_BH1750 : public DeviceBase
{   
  public:
    Device_BH1750();
    ~Device_BH1750();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    hp_BH1750 BH1750;

  private:
    bool dataAvailable;
  

};

#endif
