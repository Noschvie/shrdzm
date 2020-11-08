#include "Device_IM350.h"

const int messageLength = 123;
const byte firstByte = 0x7E; 
const byte lastByte = 0x7E; 
const int waitTime = 1100;

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
  return dataAvailable;
}

bool Device_IM350::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

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
  byte message[messageLength];
  char hexCode[3];

  hexCode[2] = 0;
  
  Serial.end();
//  U0C0 = BIT(UCBN) | BIT(UCBN+1) | BIT(UCSBN); // Inverse RX
  Serial.begin(115200);
//  U0C0 = BIT(UCBN) | BIT(UCBN+1) | BIT(UCSBN); // Inverse RX

  digitalWrite(atoi(deviceParameter["requestpin"]), LOW);
    
  while(Serial.available() > 0)
  {
    byte trash = Serial.read();
  }

  // enable request
  digitalWrite(atoi(deviceParameter["requestpin"]), HIGH); 
   
  unsigned long requestMillis = millis();
  while(Serial.available() < messageLength && millis()-requestMillis <= waitTime){}
  digitalWrite(atoi(deviceParameter["requestpin"]), LOW);

  for(int i = 0; i<messageLength; i++)
  {
    message[i] = Serial.read();
  }

  for(int i = 0; i<messageLength; i++)
  {
    sprintf(hexCode, "%02X", message[i]);
    code += String(hexCode);    
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
