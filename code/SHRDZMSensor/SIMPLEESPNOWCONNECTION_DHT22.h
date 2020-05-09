#ifndef SIMPLEESPNOWCONNECTION_DHT22_H
#define SIMPLEESPNOWCONNECTION_DHT22_H

//#include <Arduino.h>
#include "DeviceBase.h"

// Sensors
#include "DHTesp.h"



class SIMPLEESPNOWCONNECTION_DHT22 : public DeviceBase
{   
  public:
    SIMPLEESPNOWCONNECTION_DHT22();
    ~SIMPLEESPNOWCONNECTION_DHT22();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    DHTesp dht;

  private:
  

};

#endif
