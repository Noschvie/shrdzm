#ifndef Device_DIGITALGROUND_H
#define Device_DIGITALGROUND_H

#include "DeviceBase.h"

// Sensors

class Device_DIGITALGROUND : public DeviceBase
{   
  public:
    Device_DIGITALGROUND();
    ~Device_DIGITALGROUND();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:

  private:
  

};

#endif
