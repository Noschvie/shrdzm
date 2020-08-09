#ifndef Device_WATER_H
#define Device_WATER_H

#include "DeviceBase.h"


class Device_WATER : public DeviceBase
{   
  public:
    Device_WATER();
    ~Device_WATER();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    bool dataAvailable;

  private:


};

#endif
