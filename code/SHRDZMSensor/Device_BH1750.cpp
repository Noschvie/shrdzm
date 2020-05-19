#include "Device_BH1750.h"

Device_BH1750::Device_BH1750()
{  
  
}

Device_BH1750::~Device_BH1750()
{
  Serial.println("BH1750 Instance deleted");
}

bool Device_BH1750::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  bool avail = false;
  
  if(deviceParameter["address"] == "0x23")
    avail = BH1750.begin(BH1750_TO_GROUND);
  else
    avail = BH1750.begin(BH1750_TO_VCC);
   

  if(!avail)
    Serial.println("Sensor not found!");
    
  BH1750.start();  
}

bool Device_BH1750::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  deviceParameter["address"] = "0x23"; // default is GND to address pin, 0x5C means address pin is connetced to VCC
      
  return true;
}

SensorData* Device_BH1750::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "illuminance";

  return al;
}

SensorData* Device_BH1750::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "address";
  al->di[0].valueI = "0x23";

  return al;
}

SensorData* Device_BH1750::readParameter()
{
  SensorData *al = new SensorData(1);
  
  al->di[0].nameI = "illuminance";
  al->di[0].valueI = String(BH1750.getLux());  

  return al;
}
