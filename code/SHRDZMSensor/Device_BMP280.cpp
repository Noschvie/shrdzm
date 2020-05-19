#include "Device_BMP280.h"

Device_BMP280::Device_BMP280()
{  
  bmp_temp = bmp.getTemperatureSensor();
  bmp_pressure = bmp.getPressureSensor();  
}

Device_BMP280::~Device_BMP280()
{
  Serial.println("BMP280 Instance deleted");
}

bool Device_BMP280::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  bool avail = false;

  uint8_t address = strtoul(deviceParameter["address"], NULL, 0);

  Serial.println(address);

  avail = bmp.begin(address);   

  if(!avail)
  {
    Serial.println("Sensor not found!"); 
  }
  else
  {
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  
    bmp_temp->printSensorDetails();    
  }
}

bool Device_BMP280::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
 
  deviceParameter["address"] = "0x76";
  deviceParameter["sealevel"] = "537";

  serializeJson(deviceParameter, Serial);
      
  return true;
}

SensorData* Device_BMP280::readParameterTypes()
{
  SensorData *al = new SensorData(3);

  al->di[0].nameI = "temperature";
  al->di[1].nameI = "normpressure";
  al->di[2].nameI = "stationpressure";

  return al;
}

SensorData* Device_BMP280::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "address";
  al->di[0].valueI = "0x76";
  al->di[1].nameI = "sealevel";
  al->di[1].valueI = "537";

  return al;
}

SensorData* Device_BMP280::readParameter()
{
  sensors_event_t temp_event, pressure_event;

  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);

  // Barometrische Höhenformel:
  // Luftdruck auf Meereshöhe = Barometeranzeige / (1-Temperaturgradient*Höhe/Temperatur auf Meereshöhe in Kelvin)^(0,03416/Temperaturgradient)
  //float np = / (1-0,0065*atoi(deviceParameter["sealevel"])/
  float kelvin = 273.15 + temp_event.temperature;
  int sealevel = atoi(deviceParameter["sealevel"].as<String>().c_str());
  float factor = (float)(pow(1-0.0065*sealevel/kelvin, 5.255));
  
  float absolute_pressure = 0;
  
  absolute_pressure = pressure_event.pressure/factor;
  Serial.println( deviceParameter["sealevel"].as<String>() );
  
  SensorData *al = new SensorData(3);
  
  al->di[0].nameI = "temperature";
  al->di[0].valueI = String(temp_event.temperature);  
  al->di[1].nameI = "stationpressure";
  al->di[1].valueI = String(pressure_event.pressure);  
  al->di[2].nameI = "normpressure";
  al->di[2].valueI = String(absolute_pressure);  

  return al;
}
