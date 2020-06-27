#ifndef Device_RELAYTIMER_H
#define Device_RELAYTIMER_H

#include "DeviceBase.h"

// Sensors

class Device_RELAYTIMER : public DeviceBase
{   
  public:
    Device_RELAYTIMER();
    ~Device_RELAYTIMER();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool setAction(String action);
    bool setPostAction(String action);
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    String getValue(String data, char separator, int index);
    int port;
    bool state;
    
  private:
  

};

#endif
