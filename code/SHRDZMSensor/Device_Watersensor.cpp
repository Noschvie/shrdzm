#include "Device_Watersensor.h"

Device_Watersensor::Device_Watersensor()
{  
  
}

Device_Watersensor::~Device_Watersensor()
{
  Serial.println("Watersensor Instance deleted");
}

bool Device_Watersensor::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("pin"))
  { 
    pinMode(deviceParameter["pin"].as<uint8_t>(), INPUT);    
  }
}

bool Device_Watersensor::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  
  deviceParameter["pin"] = "12";
      
  return true;
}

SensorData* Device_Watersensor::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "water";

  return al;
}

SensorData* Device_Watersensor::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "pin";
  al->di[0].valueI = "12";

  return al;
}

SensorData* Device_Watersensor::readParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "water";
  al->di[0].valueI = digitalRead(deviceParameter["pin"].as<uint8_t>()) == 1 ? "YES" : "NO";
    
  return al;
}
