#ifndef SIMPLEESPNOWCONNECTION_DS18B20_H
#define SIMPLEESPNOWCONNECTION_DS18B20_H

#include "DeviceBase.h"

// Sensors
#include <OneWire.h>
#include <DallasTemperature.h>

class SIMPLEESPNOWCONNECTION_DS18B20 : public DeviceBase
{   
  public:
    SIMPLEESPNOWCONNECTION_DS18B20();
    ~SIMPLEESPNOWCONNECTION_DS18B20();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    OneWire oneWire;
    DallasTemperature dallas;

  private:
    String getAddressString(DeviceAddress thermometerAddress);

};

#endif
