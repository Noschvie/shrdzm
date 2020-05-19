#include "Device_ANALOG.h"

Device_ANALOG::Device_ANALOG()
{  
}

Device_ANALOG::~Device_ANALOG()
{
  Serial.println("ANALOG Instance deleted");
}

bool Device_ANALOG::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_ANALOG::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  return true;
}

SensorData* Device_ANALOG::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "analog";

  return al;
}

SensorData* Device_ANALOG::readInitialSetupParameter()
{
  SensorData *al = new SensorData(0);

  return al;
}

SensorData* Device_ANALOG::readParameter()
{  
  SensorData *al = new SensorData(1);
  
  al->di[0].nameI = "analog";
  al->di[0].valueI = String(analogRead(A0));  

  return al;
}
