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
    SIMPLEESPNOWCONNECTION_DHT22(String deviceType);
    
    bool setDeviceParameter(JsonObject obj);
    SensorData* readParameterTypes();
    SensorData* readParameter();
    
  protected:
    DHTesp dht;

  private:
  

};

#endif
