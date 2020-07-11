#ifndef Device_GW60_H
#define Device_GW60_H

#include "DeviceBase.h"

// Sensors

class Device_GW60 : public DeviceBase
{   
  public:
    Device_GW60();
    ~Device_GW60();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool loop();
    bool setAction(String action);
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    String getValue(String data, char separator, int index);
//    unsigned long et;
    bool actionSet;

  private:
  

};

#endif
