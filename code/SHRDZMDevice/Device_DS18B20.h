#ifndef Device_DS18B20_H
#define Device_DS18B20_H

#include "config/config.h"
#include "DeviceBase.h"

// Sensors
#include <OneWire.h>
#include <DallasTemperature.h>

class Device_DS18B20 : public DeviceBase
{   
  public:
    Device_DS18B20();
    ~Device_DS18B20();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable(){return dataAvailable;};
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    OneWire oneWire;
    DallasTemperature dallas;

  private:
    String getAddressString(DeviceAddress thermometerAddress);
    bool dataAvailable;

};

#endif
