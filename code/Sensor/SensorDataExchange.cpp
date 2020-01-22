#include "SensorDataExchange.h"


SensorDataExchange::DataItem::DataItem(String name, String value)
{
  nameI = name;
  valueI = value;
}

SensorDataExchange::SensorDataExchange()
{
  list = new SimpleList<DataItem>();
}

SensorDataExchange::~SensorDataExchange()
{
  delete list;
}

void SensorDataExchange::AddSensorData(String name, String value)
{
  Serial.println("class works");

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

  String r = "$[D]$"+di.nameI+":"+di.valueI;
  
/*  char payload[50];//limit is liek 200bytes, but we don't need anything close to that
  sprintf(payload, "$[D]$%s:%s", di.nameI.c_str(), di.valueI.c_str());

  //Serial.printf("payload = %s\n", payload);

  uint8_t bs[strlen(payload)];
  memcpy(bs, &payload, strlen(payload)); */
  
  return r;
}
