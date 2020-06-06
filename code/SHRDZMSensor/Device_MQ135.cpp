#include "Device_MQ135.h"

Device_MQ135::Device_MQ135()
{  
}

Device_MQ135::~Device_MQ135()
{
  Serial.println("MQ135 Instance deleted");
}

bool Device_MQ135::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_MQ135::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  return true;
}

SensorData* Device_MQ135::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "conductivity";

  return al;
}

SensorData* Device_MQ135::readInitialSetupParameter()
{
  SensorData *al = new SensorData(0);

  return al;
}

SensorData* Device_MQ135::readParameter()
{  
  SensorData *al = new SensorData(1);

  int gas = analogRead(A0);
  
  al->di[0].nameI = "conductivity";
  al->di[0].valueI = String(((float)gas/1023)*100);  

  return al;
}
