#include "configuration.h"

static DynamicJsonDocument g_configdoc(1024);

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

void Configuration::init()
{
  uint8_t pmac[6];
  WiFi.macAddress(pmac);
  m_deviceName = macToStr(pmac);

  m_deviceName.replace(":", "");
  m_deviceName.toUpperCase();
}

String Configuration::macToStr(const uint8_t* mac)
{
  char mac_addr[13];
  mac_addr[12] = 0;
  
  sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

  return String(mac_addr);
}
