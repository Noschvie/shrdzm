#include "Device_RELAYTIMER.h"

Device_RELAYTIMER::Device_RELAYTIMER()
{  
  actionParameter = docAction.to<JsonObject>();  
  actionParameter["relay1"] = "ON,OFF";
  actionParameter["relay2"] = "ON,OFF";
  actionParameter["relay3"] = "";
  actionParameter["relay4"] = "ON,OFF,SWITCH";

  Serial.println("Action Parameter set");
}

Device_RELAYTIMER::~Device_RELAYTIMER()
{
  Serial.println("RELAYTIMER Instance deleted");
}

bool Device_RELAYTIMER::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_RELAYTIMER::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  return true;
}

SensorData* Device_RELAYTIMER::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "relay";

  return al;
}

SensorData* Device_RELAYTIMER::readInitialSetupParameter()
{
  SensorData *al = new SensorData(3);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "15";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";
  al->di[2].nameI = "processtime";
  al->di[2].valueI = "1.5";

  return al;
}

SensorData* Device_RELAYTIMER::readParameter()
{  
  SensorData *al = new SensorData(1);
  
  al->di[0].nameI = "relay";
  al->di[0].valueI = "ON";  

  return al;
}
