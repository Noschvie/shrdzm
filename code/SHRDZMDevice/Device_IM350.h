#ifndef Device_IM350_H
#define Device_IM350_H

#include "DeviceBase.h"
#include "config/config.h"

#include <Crypto.h>
#include <AES.h>
#include <GCM.h>
#include <SoftwareSerial.h>

struct Vector_GCM {
    const char *name;
    uint8_t key[16];
    uint8_t *ciphertextPos;
    uint8_t authdata[16];
    uint8_t iv[12];
    uint8_t tag[12];
    size_t authsize;
    size_t datasize;
    size_t tagsize;
    size_t ivsize;
};

class Device_IM350 : public DeviceBase
{   
  public:
    Device_IM350();
    ~Device_IM350();

    enum devicetype {
      unknown,
      im350,
      am550,
      im350Wels,
      sagemcom
    };
        
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:   
    Vector_GCM Vector_SM;
    GCM<AES128> *gcmaes128 = 0;
    
    void init_vector(Vector_GCM *vect, byte *key_SM, byte *readMessage, devicetype dt);
    void decrypt_text(Vector_GCM *vect, byte *bufferResult);
    uint32_t byteToUInt32(byte array[], unsigned int startByte);
    void parse_message(byte array[], devicetype dt);
    void hexToBytes(const char* code, byte* result);  
    String hexToString(byte array[], int readCnt);
    void timeToString(char* string, size_t size);
    uint32_t writeHourValue(const char *value);
    uint32_t writeDayValue(const char *value);
    time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss);
//    devicetype dt;

    bool softwareSerialUsed;
    SoftwareSerial mySoftwareSerial;
    bool inverted;
    bool done;
    int interval;
    
    char m_cipherkey[33];
    byte m_blockCipherKey[16];

    uint32_t counter_reading_p_in;
    uint32_t counter_reading_p_out;
    uint32_t counter_reading_q_in;
    uint32_t counter_reading_q_out;
    uint32_t current_power_usage_in;
    uint32_t current_power_usage_out;    

    char meterTime[20];
};

#endif
