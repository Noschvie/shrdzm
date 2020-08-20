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
    bool isNewDataAvailable(){return dataAvailable;};
    bool loop();
    bool setAction(String action);
    bool setPostAction();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    String getValue(String data, char separator, int index);
    void setPort(bool high);
    int port;
    bool state;
    unsigned long et;
    bool actionSet;
    bool dataAvailable;
    
  private:
  

};

#endif
