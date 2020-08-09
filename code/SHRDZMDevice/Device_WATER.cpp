#include "Device_WATER.h"

Device_WATER::Device_WATER()
{  
  dataAvailable = true;
}

Device_WATER::~Device_WATER()
{
  Serial.println("Watersensor Instance deleted");
}

bool Device_WATER::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("pin"))
  { 
    pinMode(deviceParameter["pin"].as<uint8_t>(), INPUT);    
  }
}

bool Device_WATER::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  
  deviceParameter["pin"] = "12";
      
  return true;
}

SensorData* Device_WATER::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "water";

  return al;
}

SensorData* Device_WATER::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "900";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

bool Device_WATER::isNewDataAvailable()
{
  return dataAvailable;
}

SensorData* Device_WATER::readParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "water";
  al->di[0].valueI = digitalRead(deviceParameter["pin"].as<uint8_t>()) == 1 ? "YES" : "NO";

  dataAvailable = false;
    
  return al;
}
