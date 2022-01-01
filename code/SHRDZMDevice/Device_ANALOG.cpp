#include "Device_ANALOG.h"

Device_ANALOG::Device_ANALOG()
{  
  dataAvailable = false; 
  deviceTypeName = "ANALOG"; 
}

Device_ANALOG::~Device_ANALOG()
{
  DLN(F("ANALOG Instance deleted"));
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
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_ANALOG::readParameter()
{  
  SensorData *al = new SensorData(1);
  
  al->di[0].nameI = "analog";
  al->di[0].valueI = String(analogRead(A0));  

  dataAvailable = false;

  return al;
}
