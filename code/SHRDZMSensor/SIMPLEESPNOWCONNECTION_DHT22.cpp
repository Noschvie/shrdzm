#include "SIMPLEESPNOWCONNECTION_DHT22.h"

SIMPLEESPNOWCONNECTION_DHT22::SIMPLEESPNOWCONNECTION_DHT22()
{  
}

SIMPLEESPNOWCONNECTION_DHT22::SIMPLEESPNOWCONNECTION_DHT22(String deviceType) 
{
  PrintText(deviceType);
}

bool SIMPLEESPNOWCONNECTION_DHT22::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

/*  Serial.println("setDeviceParameter vom SIMPLEESPNOWCONNECTION_DHT22");

  String output;
  serializeJson(deviceParameter, output);

  PrintText(output);  
  */

  if(deviceParameter.containsKey("pin"))
  {
    dht.setup(deviceParameter["pin"].as<uint8_t>(), DHTesp::DHT22);
  }
}

SensorData* SIMPLEESPNOWCONNECTION_DHT22::readParameterTypes()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "temperature";
  al->di[1].nameI = "humidity";

  return al;
}

SensorData* SIMPLEESPNOWCONNECTION_DHT22::readParameter()
{
  SensorData *al = new SensorData(2);
  delay(dht.getMinimumSamplingPeriod());

  al->di[0].nameI = "temperature";
  al->di[0].valueI = String(dht.getTemperature());
  
  al->di[1].nameI = "humidity";
  al->di[1].valueI = String(dht.getHumidity());

  return al;
}
