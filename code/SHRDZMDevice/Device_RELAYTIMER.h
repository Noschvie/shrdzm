#ifndef Device_RELAYTIMER_H
#define Device_RELAYTIMER_H

#include "DeviceBase.h"

// Sensors

class Device_RELAYTIMER : public DeviceBase
{   
  public:
    Device_RELAYTIMER();
    ~Device_RELAYTIMER();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:

  private:
  

};

#endif
