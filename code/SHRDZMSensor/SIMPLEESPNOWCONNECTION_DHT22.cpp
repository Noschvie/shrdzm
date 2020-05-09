#include "SIMPLEESPNOWCONNECTION_DHT22.h"

SIMPLEESPNOWCONNECTION_DHT22::SIMPLEESPNOWCONNECTION_DHT22()
{  
}

SIMPLEESPNOWCONNECTION_DHT22::SIMPLEESPNOWCONNECTION_DHT22(String deviceType) 
{
  PrintText(deviceType);
}

SensorData* SIMPLEESPNOWCONNECTION_DHT22::readParameterTypes()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "temperature";
  al->di[1].nameI = "humidity";

  return al;
}
