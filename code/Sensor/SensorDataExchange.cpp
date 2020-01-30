#include "SensorDataExchange.h"
#include <ESP8266WiFi.h>

SensorDataExchange::DataItem::DataItem(String name, String value)
{
  nameI = name;
  valueI = value;
}

SensorDataExchange::SensorDataExchange()
{
  list = new SimpleList<DataItem>();

  uint8_t xmac[6];
  WiFi.macAddress(xmac);
  deviceName = macToStr(xmac);
  deviceName.replace(":", "");
  deviceName.toUpperCase();
}

SensorDataExchange::~SensorDataExchange()
{
  delete list;
}

void SensorDataExchange::AddSensorData(String name, String value)
{
  DataItem ndi(name, value);  
  list->add(ndi);
}

int SensorDataExchange::GetCount()
{
  return list->size();
}

String SensorDataExchange::GetMessage(int pos)
{
  DataItem di = list->get(pos);

  String r = "$[D]$"+deviceName+"$"+di.nameI+":"+di.valueI;

  return r;
}

String SensorDataExchange::macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
