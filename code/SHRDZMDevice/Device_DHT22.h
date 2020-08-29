#ifndef Device_DHT22_H
#define Device_DHT22_H

#include "DeviceBase.h"

// Sensors
#include "DHTesp.h"

class Device_DHT22 : public DeviceBase
{   
  public:
    Device_DHT22();
    ~Device_DHT22();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable(){return dataAvailable;};
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    DHTesp dht;

  private:
    bool dataAvailable;
  

};

#endif
