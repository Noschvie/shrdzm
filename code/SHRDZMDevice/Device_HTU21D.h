#ifndef Device_HTU21D_H
#define Device_HTU21D_H

#include <Wire.h>
#include "DeviceBase.h"

// Sensors
#include <HTU21D.h>

class Device_HTU21D : public DeviceBase
{   
  public:
    Device_HTU21D();
    ~Device_HTU21D();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable(){return dataAvailable;};
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    HTU21D myHTU21D;
    float toFahrenheit(float fromCelcius) { return 1.8 * fromCelcius + 32.0; };
    float toCelsius(float fromFahrenheit) { return (fromFahrenheit - 32.0) / 1.8; };

  private:
    bool dataAvailable;
  

};

#endif
