#include "Device_MQ135.h"

Device_MQ135::Device_MQ135()
{  
  deviceTypeName = "MQ135";
  dataAvailable = false;  
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
  deviceParameter["R2_Ohm"] = "22000";
  deviceParameter["ADC_Volt"] = "3.3";

  return true;
}

SensorData* Device_MQ135::readParameterTypes()
{
  SensorData *al = new SensorData(3);

  al->di[0].nameI = "conductivity";
  al->di[1].nameI = "steps";
  al->di[2].nameI = "sensoroutputvoltage";

  return al;
}

SensorData* Device_MQ135::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "300";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "25";

  return al;
}

void Device_MQ135::prepare()
{
  // enable sensor
  Serial.println("MQ135 Sensor Power enabled");

  
}

SensorData* Device_MQ135::readParameter()
{  
  SensorData *al = new SensorData(3);

  int gas = analogRead(A0);

  float u2 = 5*atoi(deviceParameter["R2_Ohm"].as<String>().c_str())/
              (atoi(deviceParameter["R1_Ohm"].as<String>().c_str())+
              (atoi(deviceParameter["R2_Ohm"].as<String>().c_str())));

  float volt = atof(deviceParameter["ADC_Volt"].as<String>().c_str())/1023*gas;
  float cond = volt * 100 / 5;
  
  al->di[0].nameI = "conductivity";
  al->di[0].valueI = String(cond,3);  
  al->di[1].nameI = "steps";
  al->di[1].valueI = String(gas);  
  al->di[2].nameI = "sensoroutputvoltage";
  al->di[2].valueI = String(volt,3);  

  dataAvailable = false;  

  return al;
}
