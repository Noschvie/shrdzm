#include "DeviceBase.h"

SensorData::DataItem::DataItem(String name, String value)
{
  nameI = name;
  valueI = value;
}

SensorData::SensorData(int size)
{
  di = new DataItem[size];
  this->size = size;
}

SensorData::~SensorData()
{
  delete [] di;
}

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
  deviceParameter = obj;
  
  return true;
}

SensorData* DeviceBase::readParameterTypes()
{
    return NULL;
}

SensorData* DeviceBase::readParameter()
{
    return NULL;
}
