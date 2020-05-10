#include "Device_BH1750.h"

Device_BH1750::Device_BH1750()
{  
  
}

Device_BH1750::~Device_BH1750()
{
  Serial.println("BH1750 Instance deleted");
}

bool Device_BH1750::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  bool avail = BH1750.begin(BH1750_TO_GROUND);

  if(!avail)
    Serial.println("Sensor not found!");
    
  BH1750.start();  
}

bool Device_BH1750::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
      
  return true;
}

SensorData* Device_BH1750::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "illuminance";

  return al;
}

SensorData* Device_BH1750::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  return al;
}

SensorData* Device_BH1750::readParameter()
{
  SensorData *al = new SensorData(1);
  
  al->di[0].nameI = "illuminance";
  al->di[0].valueI = String(BH1750.getLux());  

  return al;
}
