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
<<<<<<< HEAD
    void prepare();
=======
    bool isNewDataAvailable();
    void prepare(); 
>>>>>>> dev
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    bool done;
    SoftwareSerial swSer;
    void gotoSleep();
    void wakeup();
<<<<<<< HEAD

  private:
=======
    uint8_t calculateREPORTINGMODECheckSum();
    uint8_t calculateWORKINGPERIODCheckSum();
    void setActiveMode();
    void setWorkingPeriod();

  private:
    bool dataAvailable;
>>>>>>> dev
  

};

#endif
