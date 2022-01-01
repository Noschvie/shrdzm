#ifndef Device_DOORSENSOR_H
#define Device_DOORSENSOR_H

#include "config/config.h"
#include "DeviceBase.h"

// Sensors

class Device_DOORSENSOR : public DeviceBase
{   
  public:
    Device_DOORSENSOR();
    ~Device_DOORSENSOR();
    
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
