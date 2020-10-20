#ifndef Device_IM350_H
#define Device_IM350_H

#include "DeviceBase.h"
#include <SoftwareSerial.h>

// Sensors

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
    SoftwareSerial swSer;

  private:
    bool dataAvailable;
  

};

#endif
