#ifndef Device_FC04_H
#define Device_FC04_H

#include "DeviceBase.h"

// Sensors

class Device_FC04 : public DeviceBase
{   
  public:
    Device_FC04();
    ~Device_FC04();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:

  private:
  

};

#endif
