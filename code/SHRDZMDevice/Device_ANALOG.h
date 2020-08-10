#ifndef Device_ANALOG_H
#define Device_ANALOG_H

#include "DeviceBase.h"

// Sensors

class Device_ANALOG : public DeviceBase
{   
  public:
    Device_ANALOG();
    ~Device_ANALOG();
    
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
