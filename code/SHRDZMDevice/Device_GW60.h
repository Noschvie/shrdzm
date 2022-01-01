#ifndef Device_GW60_H
#define Device_GW60_H

#include "config/config.h"
#include "DeviceBase.h"
#include <FS.H>

// Sensors

class Device_GW60 : public DeviceBase
{   
  public:
    Device_GW60();
    ~Device_GW60();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool loop();
    bool isNewDataAvailable();
    bool setAction(String action);
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    String getValue(String data, char separator, int index);
    void countposition();
    void currentdirection();
    void currentposition();
    void newposition();
    void savemaxcount();
    void readmaxcount();
    boolean isNumeric(String str);
    void up();
    void down();
    bool actionSet;
    String m_laststate;
    String m_lastaction;
    String m_moving;
    bool remote;
    unsigned long lastActionStart;
    int dir, shadeposition, count, maxcount, newpercentage, counted, pinD6, address;
    float percentage = 0;
    

  private:
  

};

#endif
