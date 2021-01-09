#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "config/config.h"

class Configuration
{
  public:
    Configuration();
    ~Configuration();
    void init();
    String macToStr(const uint8_t* mac);
    
    String m_deviceName;

};

#endif
