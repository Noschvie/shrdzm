#include "Device_MQ135.h"

Device_MQ135::Device_MQ135()
{  
}

Device_MQ135::~Device_MQ135()
{
  Serial.println("MQ135 Instance deleted ");
}

bool Device_MQ135::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_MQ135::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  deviceParameter["R1_Ohm"] = "4700";
  deviceParameter["R2_Ohm"] = "10000";
  deviceParameter["ADC_Volt"] = "3.3";

  return true;
}

SensorData* Device_MQ135::readParameterTypes()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "conductivity";
  al->di[1].nameI = "steps";

  return al;
}

SensorData* Device_MQ135::readInitialSetupParameter()
{
  SensorData *al = new SensorData(3);

  al->di[0].nameI = "R1_Ohm";
  al->di[0].valueI = "4700";
  al->di[1].nameI = "R2_Ohm";
  al->di[1].valueI = "10000";
  al->di[2].nameI = "ADC_Volt";
  al->di[2].valueI = "3.3";

  return al;
}

SensorData* Device_MQ135::readParameter()
{  
  SensorData *al = new SensorData(2);

  int gas = analogRead(A0);

  float u2 = 5*atoi(deviceParameter["R2_Ohm"].as<String>().c_str())/
              (atoi(deviceParameter["R1_Ohm"].as<String>().c_str())+
              (atoi(deviceParameter["R2_Ohm"].as<String>().c_str())));

  float cond = atof(deviceParameter["ADC_Volt"].as<String>().c_str())/1023*gas;
  
//  Serial.println("u2 = "+String(u2));
//  Serial.println("u2 = "+String(cond,3));

//  Serial.println("analog value = "+String(gas));
  
  al->di[0].nameI = "conductivity";
  al->di[0].valueI = String(cond,3);  
  al->di[1].nameI = "steps";
  al->di[1].valueI = String(gas);  

  return al;
}
