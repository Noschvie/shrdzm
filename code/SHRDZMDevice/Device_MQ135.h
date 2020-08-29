#ifndef Device_MQ135_H
#define Device_MQ135_H

#include "DeviceBase.h"

// Sensors

class Device_MQ135 : public DeviceBase
{   
  public:
    Device_MQ135();
    ~Device_MQ135();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    void prepare();
    bool isNewDataAvailable(){return dataAvailable;};
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:

  private:
    bool dataAvailable;
  

};

#endif
