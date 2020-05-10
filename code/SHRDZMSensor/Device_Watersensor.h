#ifndef Device_Watersensor_H
#define Device_Watersensor_H

#include "DeviceBase.h"


class Device_Watersensor : public DeviceBase
{   
  public:
    Device_Watersensor();
    ~Device_Watersensor();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:


  private:


};

#endif
