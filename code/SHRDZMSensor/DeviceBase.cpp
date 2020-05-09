#include "DeviceBase.h"

DeviceBase::DeviceBase(String text)
{
  Serial.println("not implemented!");
}

void DeviceBase::PrintText(String text)
{
  Serial.println(text);
}

bool DeviceBase::setDeviceParameter(JsonObject obj)
{  



  String output;
  serializeJson(obj, output);

  PrintText(output);
  
  return true;
}
