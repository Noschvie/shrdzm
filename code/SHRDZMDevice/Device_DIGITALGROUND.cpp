#include "Device_DIGITALGROUND.h"

Device_DIGITALGROUND::Device_DIGITALGROUND()
{  
  dataAvailable = true;  
}

Device_DIGITALGROUND::~Device_DIGITALGROUND()
{
  Serial.println("DIGITAL Instance deleted");
}

bool Device_DIGITALGROUND::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("pin"))
  {
    if(deviceParameter["pin"].as<uint8_t>() != 99)
      pinMode(deviceParameter["pin"].as<uint8_t>(), INPUT_PULLUP);
  }
}

bool Device_DIGITALGROUND::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  
  deviceParameter["pin"] = "99";
  
  return true;
}

SensorData* Device_DIGITALGROUND::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "state";

  return al;
}

SensorData* Device_DIGITALGROUND::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_DIGITALGROUND::readParameter()
{  
  SensorData *al = new SensorData(1);

//  int sensorVal = digitalRead(deviceParameter["pin"].as<uint8_t>());

  if(deviceParameter["pin"].as<uint8_t>() < 16)
  {
    al->di[0].nameI = "state";
    al->di[0].valueI = digitalRead(deviceParameter["pin"].as<uint8_t>()) ? "open" : "closed";
  }
  else
  {
    al->di[0].nameI = "state";
    al->di[0].valueI = "error";
  }

  dataAvailable = false;
  
  return al;
}
