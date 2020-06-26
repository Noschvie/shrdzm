#ifndef Device_RELAY_H
#define Device_RELAY_H

#include "DeviceBase.h"

// Sensors

class Device_RELAY : public DeviceBase
{   
  public:
    Device_RELAY();
    ~Device_RELAY();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:

  private:
  

};

#endif
