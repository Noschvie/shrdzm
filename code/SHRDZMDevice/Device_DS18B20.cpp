#include "Device_DS18B20.h"

Device_DS18B20::Device_DS18B20()
{  
  dataAvailable = false;    
}

Device_DS18B20::~Device_DS18B20()
{
  Serial.println("BH1750 Instance deleted");
}

bool Device_DS18B20::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);

  if(deviceParameter.containsKey("pin"))
  {
    oneWire.begin(deviceParameter["pin"].as<uint8_t>());
    dallas.setOneWire(&oneWire);
    dallas.begin();    
  }
}

bool Device_DS18B20::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  
  deviceParameter["pin"] = "12";
      
  return true;
}

SensorData* Device_DS18B20::readParameterTypes()
{
  SensorData *al = new SensorData(dallas.getDeviceCount());

  DeviceAddress thermometerAddress;

  for (int i = 0; i < dallas.getDeviceCount(); i++) 
  {
    if(dallas.getAddress(thermometerAddress, i))
    {
      al->di[i].nameI = "temperature_"+getAddressString(thermometerAddress);
    }
  }

  return al;
}

SensorData* Device_DS18B20::readInitialSetupParameter()
{
  SensorData *al = new SensorData(2);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";

  return al;
}

SensorData* Device_DS18B20::readParameter()
{
  SensorData *al = new SensorData(dallas.getDeviceCount());

  DeviceAddress thermometerAddress;

  for (int i = 0; i < dallas.getDeviceCount(); i++) 
  {
    if(dallas.getAddress(thermometerAddress, i))
    {
      dallas.setResolution(thermometerAddress, 12);
    }
  }

  dallas.requestTemperatures();

  for (int i = 0; i < dallas.getDeviceCount(); i++) 
  {
      if(dallas.getAddress(thermometerAddress, i))
      {        
        al->di[i].nameI = "temperature_"+getAddressString(thermometerAddress);
        al->di[i].valueI = String(dallas.getTempC(thermometerAddress));
      }
  }
  
  return al;
}

String Device_DS18B20::getAddressString(DeviceAddress thermometerAddress)
{
  String a;

  for(int i = 5; i<8; i++)
  {
    if( thermometerAddress[i] < 16)
      a+= "0";
    a+= String(thermometerAddress[i], HEX);
  }

  dataAvailable = false;  

  return a;
}
