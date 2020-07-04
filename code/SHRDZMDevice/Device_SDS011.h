#ifndef Device_SDS011_H
#define Device_SDS011_H

#include "DeviceBase.h"
#include <SoftwareSerial.h>

// Sensors

class Device_SDS011 : public DeviceBase
{   
  public:
    Device_SDS011();
    ~Device_SDS011();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    void prepare(); 
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    bool done;
    SoftwareSerial swSer;
    void gotoSleep();
    void wakeup();

  private:
  

};

#endif
