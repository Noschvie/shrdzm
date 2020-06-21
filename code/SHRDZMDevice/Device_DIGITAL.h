#ifndef Device_DIGITAL_H
#define Device_DIGITAL_H

#include "DeviceBase.h"

// Sensors

class Device_DIGITAL : public DeviceBase
{   
  public:
    Device_DIGITAL();
    ~Device_DIGITAL();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:

  private:
  

};

#endif
