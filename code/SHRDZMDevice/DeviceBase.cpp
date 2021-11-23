#include "DeviceBase.h"

ConfigData::ConfigDataItem::ConfigDataItem(String value)
{
  valueI = value;
}

ConfigData::ConfigData(int size)
{
  di = new ConfigDataItem[size];
  this->size = size;
}

ConfigData::~ConfigData()
{
  delete [] di;
}

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

void SensorData::setDataItem(const char *name, const char *value)
{
  for(int i = 0; i< size; i++)
  {
    if(di[i].nameI == name)
      di[i].valueI = value;
  }  
}

String SensorData::getDataItem(String name)
{
  for(int i = 0; i< size; i++)
  {
    if(di[i].nameI == name)
      return di[i].valueI;
  }

  return "";
}

const char* SensorData::getDataItemPtr(const char *name)
{
  for(int i = 0; i< size; i++)
  {
    if(strcmp(di[i].nameI.c_str(),name) == 0)
      return di[i].valueI.c_str();
  }

  return NULL;
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

void DeviceBase::setConfigurationObject(DynamicJsonDocument *configurationObject)
{
  m_pConfigurationObject = configurationObject;
};

JsonObject DeviceBase::getDeviceParameter()
{
  return deviceParameter;
}

JsonObject DeviceBase::getActionParameter()
{
  return actionParameter;
}

SensorData* DeviceBase::readParameterTypes()
{
    return NULL;
}

SensorData* DeviceBase::readParameter()
{
    return NULL;
}
