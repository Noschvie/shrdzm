#include "Device_SDS011_BME280.h"

#define LEN 9

static const byte SLEEPCMD[19] = 
{
  0xAA, // head
  0xB4, // command id
  0x06, // data byte 1
  0x01, // data byte 2 (set mode)
  0x00, // data byte 3 (sleep)
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1)
  0xFF, // data byte 15 (device id byte 2)
  0x05, // checksum
  0xAB  // tail
};
 
static const byte REPORTINGMODE[19] = 
{
  0xAA, // head
  0xB4, // command id
  0x02, // data byte 1
  0x01, // data byte 2 (set mode)
  0x00, // data byte 3 (active)
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1)
  0xFF, // data byte 15 (device id byte 2)
  0x01, // checksum
  0xAB  // tail
};

static const byte WORKINGPERIOD[19] = 
{
  0xAA, // head
  0xB4, // command id
  0x08, // data byte 1
  0x01, // data byte 2 (set mode)
  0x00, // data byte 3 (active)
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1)
  0xFF, // data byte 15 (device id byte 2)
  0x07, // checksum
  0xAB  // tail
};

Device_SDS011_BME280::Device_SDS011_BME280()
{    
  deviceTypeName = "SDS011_BME280";
  
  done = false;
  dataAvailable = false;  
  sensorAvailable = false;      
}

Device_SDS011_BME280::~Device_SDS011_BME280()
{
  Serial.println("SDS011_BME280 Instance deleted");
}

bool Device_SDS011_BME280::isNewDataAvailable()
{
  return false;
}

bool Device_SDS011_BME280::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
  bool avail = false;

  uint8_t address = strtoul(deviceParameter["address"], NULL, 0);
  avail = bme.begin(address);   

  if(!avail)
  {
    Serial.println("Sensor not found!"); 
    sensorAvailable = false;    
  }
  else
  {
    sensorAvailable = true;    
  }

  if(deviceParameter.containsKey("RX") && deviceParameter.containsKey("TX"))
  { 
    swSer.begin(9600, SWSERIAL_8N1, atoi(deviceParameter["RX"]), atoi(deviceParameter["TX"]), false);    
  }

  
}

void Device_SDS011_BME280::prepare()
{
  wakeup(); 
  delay(200);
  setActiveMode();
  delay(200);
  setWorkingPeriod();
  delay(300);
}

bool Device_SDS011_BME280::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  deviceParameter["RX"] = "12";
  deviceParameter["TX"] = "2";
  deviceParameter["address"] = "0x76";
  deviceParameter["sealevel"] = "537";

  return true;
}

SensorData* Device_SDS011_BME280::readParameterTypes()
{
  SensorData *al = new SensorData(7);

  al->di[0].nameI = "PM25";
  al->di[1].nameI = "PM10";
  al->di[2].nameI = "temperature";
  al->di[3].nameI = "humidity";
  al->di[4].nameI = "normpressure";
  al->di[5].nameI = "stationpressure";
  al->di[6].nameI = "error";

  return al;
}

SensorData* Device_SDS011_BME280::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "600";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "20";

  return al;
}

void Device_SDS011_BME280::setActiveMode() 
{   
  for (uint8_t i = 0; i < 19; i++) 
  {
    swSer.write(REPORTINGMODE[i]);
  }
  swSer.flush();
  while (swSer.available() > 0) 
  {
    Serial.print(swSer.read());
  }  

  Serial.println();
}

void Device_SDS011_BME280::setWorkingPeriod() 
{   
  for (uint8_t i = 0; i < 19; i++) 
  {
    swSer.write(WORKINGPERIOD[i]);
  }
  swSer.flush();
  while (swSer.available() > 0) 
  {
    Serial.print(swSer.read());
  }  

  Serial.println();
}


void Device_SDS011_BME280::wakeup() 
{
  Serial.println("will wake up...");
  
  for (uint8_t i = 0; i < 19; i++) 
  {
    if (i == 4 || i == 17) 
    {
      swSer.write(SLEEPCMD[i] + 1);
    } 
    else 
    {
      swSer.write(SLEEPCMD[i]);
    }
  }
  
  swSer.flush();
  while (swSer.available() > 0) 
  {
    Serial.print(swSer.read());
  }

  Serial.println();
}

void Device_SDS011_BME280::gotoSleep()
{
  Serial.println("gotoSleep");
  
  for (uint8_t i = 0; i < 19; i++) 
  {
    swSer.write(SLEEPCMD[i]);
  }
  swSer.flush();
  while (swSer.available() > 0) 
  {
    swSer.read();
  }  
}

SensorData* Device_SDS011_BME280::readParameter()
{  
  unsigned long w = millis();
  
  SensorData *al = new SensorData(7);

  int i;
  unsigned char checksum;
  unsigned char incomingByte = 0;
  unsigned char buf[LEN];
  float PM2_5Val = 0;
  float PM10Val = 0;

  while(!done)
  {
    if (swSer.available() > 0) 
    {
      incomingByte = swSer.read();
      if (incomingByte == 0xAA) 
      {
        swSer.readBytes(buf, LEN);
      
        if ((buf[0] == 0xC0) && (buf[8] == 0xAB)) 
        {
          for (i=1; i<=6; i++) 
          {
            checksum = checksum + buf[i];
          }
          if (checksum != buf[7]) 
          {
            PM2_5Val=((buf[2]<<8) + buf[1])/10.0;
            PM10Val=((buf[4]<<8) + buf[3])/10.0;

            al->di[0].nameI = "PM25";
            al->di[0].valueI = String(PM2_5Val);  
            al->di[1].nameI = "PM10";
            al->di[1].valueI = String(PM10Val);  

            done = true;
          }
        }
      }
    }

    if(millis() > w+1500)
      done = true;
  }

  dataAvailable = false;

  // BMP280
  if(sensorAvailable)
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
    
    al->di[2].nameI = "temperature";
    al->di[2].valueI = String(temperature);  
    al->di[3].nameI = "humidity";
    al->di[3].valueI = String(humidity);  
    al->di[4].nameI = "stationpressure";
    al->di[4].valueI = String(pressure);  
    al->di[5].nameI = "normpressure";
    al->di[5].valueI = String(absolute_pressure);  
    al->di[6].nameI = "error";
    al->di[6].valueI = "NO";      
  }
  else
  {
    al->di[2].nameI = "temperature";
    al->di[2].valueI = "0.0";  
    al->di[3].nameI = "stationpressure";
    al->di[3].valueI = "0.0";  
    al->di[4].nameI = "normpressure";
    al->di[4].valueI = "0.0";  
    al->di[5].nameI = "humidity";
    al->di[5].valueI = "0.0";  
    al->di[6].nameI = "error";
    al->di[6].valueI = "Sensor not working";          
  }
  
  gotoSleep();
  
  return al;
}
