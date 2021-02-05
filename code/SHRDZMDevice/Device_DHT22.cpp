#include "Device_DHT22.h"

Device_DHT22::Device_DHT22()
{  
  dataAvailable = false;    
  deviceTypeName = "DHT22";
}

Device_DHT22::~Device_DHT22()
{
  Serial.println("DHT22 Instance deleted");
}

bool Device_DHT22::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("pin"))
  {
    dht.setup(deviceParameter["pin"].as<uint8_t>(), DHTesp::DHT22);
  }
}

bool Device_DHT22::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();  
  deviceParameter["pin"] = "12";

  return true;
}

SensorData* Device_DHT22::readParameterTypes()
{
  SensorData *al = new SensorData(5);

  al->di[0].nameI = "temperature";
  al->di[1].nameI = "humidity";
  al->di[2].nameI = "dewpoint";
  al->di[3].nameI = "absolutehumidity";
  al->di[4].nameI = "heatindex";  
  

  return al;
}

SensorData* Device_DHT22::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_DHT22::readParameter()
{
  SensorData *al = NULL;

  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  if(!isnan(temperature) || !isnan(humidity))
  {
    al = new SensorData(5);
    
    al->di[0].nameI = "temperature";
    al->di[0].valueI = String(temperature);
    
    al->di[1].nameI = "humidity";
    al->di[1].valueI = String(humidity);

    al->di[2].nameI = "dewpoint";
    al->di[2].valueI = String(dht.computeDewPoint(temperature, humidity));

    al->di[3].nameI = "absolutehumidity";
    al->di[3].valueI = String(dht.computeAbsoluteHumidity(temperature, humidity));    

    al->di[4].nameI = "heatindex";
    al->di[4].valueI = String(dht.computeHeatIndex(temperature, humidity));    
  }
  
  dataAvailable = false;

  return al;
}
