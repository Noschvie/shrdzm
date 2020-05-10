#ifndef SIMPLEESPNOWCONNECTION_BH1750_H
#define SIMPLEESPNOWCONNECTION_BH1750_H

#include <Wire.h>
#include "DeviceBase.h"

// Sensors
#include <hp_BH1750.h>

class SIMPLEESPNOWCONNECTION_BH1750 : public DeviceBase
{   
  public:
    SIMPLEESPNOWCONNECTION_BH1750();
    ~SIMPLEESPNOWCONNECTION_BH1750();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    hp_BH1750 BH1750;

  private:
  

};

#endif
