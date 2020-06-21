#include "Device_HTU21D.h"

Device_HTU21D::Device_HTU21D()
{  
}

Device_HTU21D::~Device_HTU21D()
{
  Serial.println("HTU21D Instance deleted");
}

bool Device_HTU21D::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_HTU21D::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
      
  return true;
}

SensorData* Device_HTU21D::readParameterTypes()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "humidity";
  al->di[1].nameI = "temperature";

  return al;
}

SensorData* Device_HTU21D::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_HTU21D::readParameter()
{
  myHTU21D.begin();
  
  SensorData *al = new SensorData(2);
  
  al->di[0].nameI = "humidity";
  al->di[0].valueI = String(myHTU21D.readHumidity());  
  al->di[1].nameI = "temperature";
  al->di[1].valueI = String(myHTU21D.readTemperature());  

  return al;
}
