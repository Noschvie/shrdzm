/*
  SHRDZMGateway

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 09 Jan 2021
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#define DEBUG_SHRDZM

#include "config/config.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
Configuration configuration;

void setup() 
{
  Serial.begin(SERIALBAUD);
  Serial.println();

  configuration.init();

  if(checkAP())
  {
    startConfigurationAP(configuration.m_deviceName);
    DLN("Configuration AP for "+configuration.m_deviceName+" started...");
  }


}

void loop() 
{
  helperLoop();
}
