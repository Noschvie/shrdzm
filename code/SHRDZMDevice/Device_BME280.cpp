#include "Device_BME280.h"

Device_BME280::Device_BME280()
{  
  dataAvailable = false;  
}

Device_BME280::~Device_BME280()
{
  Serial.println("BME280 Instance deleted");
}

bool Device_BME280::isNewDataAvailable()
{
  return dataAvailable;
}

bool Device_BME280::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  bool avail = false;

  uint8_t address = strtoul(deviceParameter["address"], NULL, 0);

  avail = bme.begin(address);   

  if(!avail)
  {
    Serial.println("Sensor not found!"); 
  }
  else
  {
  }
}

bool Device_BME280::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
 
  deviceParameter["address"] = "0x76";
  deviceParameter["sealevel"] = "537";

  return true;
}

SensorData* Device_BME280::readParameterTypes()
{
  SensorData *al = new SensorData(4);

  al->di[0].nameI = "temperature";
  al->di[1].nameI = "humidity";
  al->di[2].nameI = "normpressure";
  al->di[3].nameI = "stationpressure";

  return al;
}

SensorData* Device_BME280::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_BME280::readParameter()
{
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();

  // Barometrische Höhenformel:
  // Luftdruck auf Meereshöhe = Barometeranzeige / (1-Temperaturgradient*Höhe/Temperatur auf Meereshöhe in Kelvin)^(0,03416/Temperaturgradient)
  float kelvin = 273.15 + temperature;
  int sealevel = atoi(deviceParameter["sealevel"].as<String>().c_str());
  float factor = (float)(pow(1-0.0065*sealevel/kelvin, 5.255));
  
  float absolute_pressure = 0;
  
  absolute_pressure = pressure/factor;
  
  SensorData *al = new SensorData(4);
  
  al->di[0].nameI = "temperature";
  al->di[0].valueI = String(temperature);  
  al->di[1].nameI = "humidity";
  al->di[1].valueI = String(humidity);  
  al->di[2].nameI = "stationpressure";
  al->di[2].valueI = String(pressure);  
  al->di[3].nameI = "normpressure";
  al->di[3].valueI = String(absolute_pressure);  

  dataAvailable = false;

  return al;
}
