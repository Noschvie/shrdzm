#include "Device_HTU21D.h"

Device_HTU21D::Device_HTU21D()
{  
  myHTU21D = new HTU21D(HTU21D_RES_RH12_TEMP14);
}

Device_HTU21D::~Device_HTU21D()
{
  Serial.println("HTU21D Instance deleted");
  delete myHTU21D;
}

bool Device_HTU21D::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  myHTU21D->begin();
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
  SensorData *al = new SensorData(1);

  return al;
}

SensorData* Device_HTU21D::readParameter()
{
  SensorData *al = new SensorData(2);
  
  al->di[0].nameI = "humidity";
  al->di[0].valueI = String(myHTU21D->readHumidity());  
  al->di[1].nameI = "temperature";
  al->di[1].valueI = String(myHTU21D->readTemperature());  

  return al;
}
