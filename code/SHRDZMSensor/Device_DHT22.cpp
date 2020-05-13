#include "Device_DHT22.h"

Device_DHT22::Device_DHT22()
{  
  
}

Device_DHT22::~Device_DHT22()
{
  Serial.println("DHT22 Instance deleted");
}

bool Device_DHT22::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("pin"))
  {
    dht.setup(deviceParameter["pin"].as<uint8_t>(), DHTesp::DHT22);
  }
}

bool Device_DHT22::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  
  deviceParameter["pin"] = "12";
  
  return true;
}

SensorData* Device_DHT22::readParameterTypes()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "temperature";
  al->di[1].nameI = "humidity";

  return al;
}

SensorData* Device_DHT22::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  al->di[1].nameI = "pin";
  al->di[1].valueI = "12";

  return al;
}

SensorData* Device_DHT22::readParameter()
{
  SensorData *al = new SensorData(2);
  delay(dht.getMinimumSamplingPeriod());

  al->di[0].nameI = "temperature";
  al->di[0].valueI = String(dht.getTemperature());
  
  al->di[1].nameI = "humidity";
  al->di[1].valueI = String(dht.getHumidity());

  return al;
}
