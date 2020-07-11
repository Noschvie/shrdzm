#include "Device_GW60.h"

#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15


Device_GW60::Device_GW60()
{  
  actionParameter = docAction.to<JsonObject>();  
  actionParameter["setpositionpercent"] = "10,20,30,40,50,60,70,80,90,100";

  actionSet = false;

  pinMode(D5, INPUT);         // Impulszähler
  pinMode(D3, INPUT_PULLUP);  // Motor fährt hoch
  pinMode(D4, INPUT_PULLUP);  // Motor fährt runter
  pinMode(D1, OUTPUT);        // Taste für runter
  digitalWrite(D1, HIGH);     // Setze Taste auf HIGH
  pinMode(D2, OUTPUT);        // Taste für hoch
  digitalWrite(D2, HIGH);     // Setze Taste auf HIGH
  pinMode(D6, INPUT_PULLUP);  // Test-Taste für Debug-Tests
  
//  et = millis() + 1000 * 10;  
}

Device_GW60::~Device_GW60()
{
  Serial.println("GW60 Instance deleted");
}

bool Device_GW60::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_GW60::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  return true;
}

bool Device_GW60::setAction(String action)
{
  String sCommand = getValue(action, ':', 0);
  
  if(sCommand == "setpositionpercent")
  {
    Serial.printf("Position set to : %s\n",getValue(action, ':', 1).c_str());
  }

  actionSet = true;
    
  return true;
}

bool Device_GW60::loop()
{
  if(actionSet)
  {
    
  }
  
/*  if(actionSet && millis() < et)
    return false;
*/
  Serial.printf("loop done\n");
  
  return true;
}

SensorData* Device_GW60::readParameterTypes()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "positionpercent";

  return al;
}

SensorData* Device_GW60::readInitialSetupParameter()
{
  SensorData *al = new SensorData(3);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "0";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";
  al->di[2].nameI = "processtime";
  al->di[2].valueI = "0";

  return al;
}

SensorData* Device_GW60::readParameter()
{  
  return NULL;
}

String Device_GW60::getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++)
  {
    if(data.charAt(i)==separator || i==maxIndex)
    {
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
