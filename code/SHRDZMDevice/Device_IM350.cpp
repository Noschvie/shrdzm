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
  if(deviceParameter.containsKey("requestpin"))
  {
    pinMode(atoi(deviceParameter["requestpin"]), OUTPUT);
    digitalWrite(atoi(deviceParameter["requestpin"]), LOW);  
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
  deviceParameter["requestpin"] = "5";

  return true;
}

SensorData* Device_IM350::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "encoded";

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
  const int waitTime = 1100;
  unsigned char incomingByte = 0;
  String code;
  bool dataWaitDone = false;
  bool dataError = false;
  
  // cleanup serial interface
  while(swSer.available() > 0)
  {
    byte trash = swSer.read();
  }

  // enable request
  digitalWrite(atoi(deviceParameter["requestpin"]), HIGH);  

  unsigned long requestMillis = millis();
  while(!dataWaitDone)
  {
    if(swSer.available() > 0)
      dataWaitDone = true;
    else
    {
      if(millis()-requestMillis >= waitTime)
      {
        dataWaitDone = true;
        dataError = true;
      }
    }
  }

  if(dataError)
  {
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "no data read";  

    return al;  
  }

//  digitalWrite(atoi(deviceParameter["requestpin"]), LOW);  

  requestMillis = millis();  
  while(!done)
  {
    if (swSer.available() > 0)
    {
      incomingByte = swSer.read();
      if(incomingByte < 16)
        code += "0";
        
      code += String(incomingByte, HEX);      
    }
    if(millis()-requestMillis >= 200)
      done = true;
  }

  if(code != "")
  {
    al->di[0].nameI = "encoded";
    al->di[0].valueI = code;  
  }
  else
  {
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "no data read";  
  }

  return al;  
}
/*SensorData* Device_IM350::readParameter()
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
    al->di[0].nameI = "encoded";
    al->di[0].valueI = code;  
  }
  else
  {
    al->di[0].nameI = "error";
    al->di[0].valueI = "no data read";  
  }
  return al;
}*/
