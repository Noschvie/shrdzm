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
  SensorData *al = new SensorData(5);

  al->di[0].nameI = "humidity";
  al->di[1].nameI = "temperature";
  al->di[2].nameI = "absolutehumidity";
  al->di[3].nameI = "dewpoint";
  al->di[4].nameI = "heatindex";  

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
  
  SensorData *al = new SensorData(5);

  float temperature = myHTU21D.readTemperature();
  float percentHumidity  = myHTU21D.readHumidity();
  float absHumidity;
  float absTemperature;
  float hi;
  
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

  // heat index
  temperature = toFahrenheit(temperature);
  hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (percentHumidity * 0.094));
  
  if (hi > 79)
  {
    hi = -42.379 +
       2.04901523 * temperature +
       10.14333127 * percentHumidity +
       -0.22475541 * temperature * percentHumidity +
       -0.00683783 * pow(temperature, 2) +
       -0.05481717 * pow(percentHumidity, 2) +
       0.00122874 * pow(temperature, 2) * percentHumidity +
       0.00085282 * temperature * pow(percentHumidity, 2) +
       -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);

    if ((percentHumidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
      hi -= ((13.0 - percentHumidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

    else if ((percentHumidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
      hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
  }  

  al->di[4].nameI = "heatindex";
  al->di[4].valueI = String(toCelsius(hi));  

  dataAvailable = false;  

  return al;
}
