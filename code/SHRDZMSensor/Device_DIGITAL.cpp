#include "Device_DIGITAL.h"

Device_DIGITAL::Device_DIGITAL()
{  
}

Device_DIGITAL::~Device_DIGITAL()
{
  Serial.println("DIGITAL Instance deleted");
}

bool Device_DIGITAL::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("pin"))
  {
    if(deviceParameter["pin"].as<uint8_t>() != 99)
      pinMode(deviceParameter["pin"].as<uint8_t>(), INPUT);
  }
}

bool Device_DIGITAL::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  
  deviceParameter["pin"] = "99";
  
  return true;
}

SensorData* Device_DIGITAL::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "state";

  return al;
}

SensorData* Device_DIGITAL::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "pin";
  al->di[0].valueI = "99";

  return al;
}

SensorData* Device_DIGITAL::readParameter()
{  
  SensorData *al = new SensorData(1);

//  int sensorVal = digitalRead(deviceParameter["pin"].as<uint8_t>());

  if(deviceParameter["pin"].as<uint8_t>() < 16)
  {
    al->di[0].nameI = "state";
    al->di[0].valueI = digitalRead(deviceParameter["pin"].as<uint8_t>()) ? "high" : "low";
  }
  else
  {
    al->di[0].nameI = "state";
    al->di[0].valueI = "error";
  }
  
  return al;
}
