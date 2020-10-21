#include "Device_IM350.h"


Device_IM350::Device_IM350()
{    
  done = false;
  dataAvailable = false;  
}

Device_IM350::~Device_IM350()
{
  Serial.println("IM350 Instance deleted");
}

bool Device_IM350::isNewDataAvailable()
{
 // return true;
  return dataAvailable;
}

bool Device_IM350::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("RX"))
  { 
    swSer.begin(115200, SWSERIAL_8N1, atoi(deviceParameter["RX"]), 99, true);    
  }
}

void Device_IM350::prepare()
{
}

bool Device_IM350::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  deviceParameter["RX"] = "4";

  return true;
}

SensorData* Device_IM350::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "code";

  return al;
}

SensorData* Device_IM350::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";

  return al;
}

SensorData* Device_IM350::readParameter()
{    
  SensorData *al = new SensorData(1);
  bool foundStart = false;
  unsigned char incomingByte = 0;
  String code;
  char hexCode[4];

  hexCode [3] = 0;
  unsigned long w = millis();

  while(!done)
  {
    if (swSer.available() > 0) 
    {
      incomingByte = swSer.read();
      if (incomingByte == 0x7E) 
      {
        if(!foundStart)
        {
          foundStart = true;
        }
        else
          done = true;
      }

      if(foundStart)
      {
        sprintf(hexCode, "%02X ", incomingByte);
        code += String(hexCode);
      }
    }

    if(millis() > w+500)
      done = true;
  }

  dataAvailable = false;

  if(code != "")
  {
    al->di[0].nameI = "code";
    al->di[0].valueI = code;  
  }
  else
  {
    al->di[0].nameI = "error";
    al->di[0].valueI = "no data read";  
  }
  return al;
}
