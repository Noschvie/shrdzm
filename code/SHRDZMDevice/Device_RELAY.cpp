#include "Device_RELAY.h"

Device_RELAY::Device_RELAY()
{  
}

Device_RELAY::~Device_RELAY()
{
  Serial.println("RELAY Instance deleted");
}

bool Device_RELAY::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_RELAY::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  return true;
}

SensorData* Device_RELAY::readParameterTypes()
{
  return NULL;
}

SensorData* Device_RELAY::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_RELAY::readParameter()
{  
  SensorData *al = new SensorData(1);
  
  al->di[0].nameI = "analog";
  al->di[0].valueI = String(analogRead(A0));  

  return al;
}
