#ifndef SensorDataExchange_h
#define SensorDataExchange_h

#include "Arduino.h"

class SensorDataExchange
{
  private:
    
  public:
    SensorDataExchange();
    void AddSensorData(String name, String value);
};

#endif
