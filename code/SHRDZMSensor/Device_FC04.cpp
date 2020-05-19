#include "Device_FC04.h"

Device_FC04::Device_FC04()
{  
}

Device_FC04::~Device_FC04()
{
  Serial.println("FC04 Instance deleted");
}

bool Device_FC04::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_FC04::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  return true;
}

SensorData* Device_FC04::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "noise";

  return al;
}

SensorData* Device_FC04::readInitialSetupParameter()
{
  SensorData *al = new SensorData(0);

  return al;
}

SensorData* Device_FC04::readParameter()
{  
  SensorData *al = new SensorData(1);
  
  al->di[0].nameI = "noise";
  al->di[0].valueI = String(analogRead(A0));  

  return al;
}
