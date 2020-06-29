#include "Device_SDS011.h"

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

Device_SDS011::Device_SDS011()
{    
  done = false;
}

Device_SDS011::~Device_SDS011()
{
  Serial.println("SDS011 Instance deleted");
}

bool Device_SDS011::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("RX") && deviceParameter.containsKey("TX"))
  { 
    swSer.begin(9600, SWSERIAL_8N1, atoi(deviceParameter["RX"]), atoi(deviceParameter["TX"]), false);    
  }
}

void Device_SDS011::prepare()
{
  wakeup(); 
}

bool Device_SDS011::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  deviceParameter["RX"] = "12";
  deviceParameter["TX"] = "2";

  return true;
}

SensorData* Device_SDS011::readParameterTypes()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "PM2.5";
  al->di[1].nameI = "PM10";

  return al;
}

SensorData* Device_SDS011::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "600";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "20";

  return al;
}

void Device_SDS011::wakeup() 
{
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
    swSer.read();
  }
}

void Device_SDS011::gotoSleep()
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

SensorData* Device_SDS011::readParameter()
{  
  unsigned long w = millis();
  
  SensorData *al = new SensorData(2);

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

            al->di[0].nameI = "PM2.5";
            al->di[0].valueI = String(PM2_5Val);  
            al->di[1].nameI = "PM10";
            al->di[1].valueI = String(PM10Val);  

/*            Serial.print("PM2.5: ");
            Serial.print(PM2_5Val);
            Serial.println(" ug/m3");
            Serial.print("PM10 : ");
            Serial.print(PM10Val);
            Serial.println(" ug/m3");
            Serial.println(); */

            done = true;
          }
        }
      }
    }

    if(millis() > w+1500)
      done = true;
  }

  gotoSleep();
  
  return al;
}
