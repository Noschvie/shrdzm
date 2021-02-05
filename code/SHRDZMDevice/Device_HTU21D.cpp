#include "Device_HTU21D.h"

Device_HTU21D::Device_HTU21D()
{  
  dataAvailable = false;  
  deviceTypeName = "HTU21D";
}

Device_HTU21D::~Device_HTU21D()
{
  Serial.println("HTU21D Instance deleted");
}

bool Device_HTU21D::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_HTU21D::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
      
  return true;
}

SensorData* Device_HTU21D::readParameterTypes()
{
  SensorData *al = new SensorData(4);

  al->di[0].nameI = "humidity";
  al->di[1].nameI = "temperature";
  al->di[2].nameI = "absolutehumidity";
  al->di[3].nameI = "dewpoint";

  return al;
}

SensorData* Device_HTU21D::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_HTU21D::readParameter()
{
  myHTU21D.begin();
  
  SensorData *al = new SensorData(4);

  float temperature = myHTU21D.readTemperature();
  float percentHumidity  = myHTU21D.readHumidity();
  float absHumidity;
  float absTemperature;
  
  al->di[0].nameI = "humidity";
  al->di[0].valueI = String(percentHumidity);  
  al->di[1].nameI = "temperature";
  al->di[1].valueI = String(temperature);  

  // absolute humidity
  absTemperature = temperature + 273.15;

  absHumidity = 6.112;
  absHumidity *= exp((17.67 * temperature) / (243.5 + temperature));
  absHumidity *= percentHumidity;
  absHumidity *= 2.1674;
  absHumidity /= absTemperature;  

  al->di[2].nameI = "absolutehumidity";
  al->di[2].valueI = String(absHumidity);  

  // dewpoint
  double A0 = 373.15 / (273.15 + (double)temperature);
  double SUM = -7.90298 * (A0 - 1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / A0))) - 1);
  SUM += 8.1328e-3 * (pow(10, (-3.49149 * (A0 - 1))) - 1);
  SUM += log10(1013.246);
  double VP = pow(10, SUM - 3) * (double)percentHumidity;
  double Td = log(VP / 0.61078); // temp var
  Td = (241.88 * Td) / (17.558 - Td);

  al->di[3].nameI = "dewpoint";
  al->di[3].valueI = String(Td);  

  dataAvailable = false;  

  return al;
}
