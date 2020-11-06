#ifndef Device_IM350_H
#define Device_IM350_H

#include "DeviceBase.h"

class Device_IM350 : public DeviceBase
{   
  public:
    Device_IM350();
    ~Device_IM350();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    bool done;

  private:
    bool dataAvailable;
      
};

#endif
