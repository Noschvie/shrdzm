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
  char buffer[4];

  String r = "[D]$"+deviceName+"$"+di.nameI+":"+di.valueI;
  int c = r.length();

  sprintf(buffer, "%03d", c);
  r = "*"+String(buffer)+r;

  return r;
}

String SensorDataExchange::macToStr(const uint8_t* mac)
{
  char mac_addr[13];
  mac_addr[12] = 0;
  
  sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

  return String(mac_addr);
}
