#ifndef Device_DIGITAL_H
#define Device_DIGITAL_H

#include "config/config.h"
#include "DeviceBase.h"

// Sensors

class Device_DIGITAL : public DeviceBase
{   
  public:
    Device_DIGITAL();
    ~Device_DIGITAL();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable(){return dataAvailable;};
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:

  private:
    bool dataAvailable;
  

};

#endif
