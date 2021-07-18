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
    uint8_t ciphertext[90];
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
      im350Wels
    };
        
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:   
    
    void init_vector(Vector_GCM *vect, const char *Vect_name, byte *key_SM, devicetype dt);
    void decrypt_text(Vector_GCM *vect);
    uint32_t byteToUInt32(byte array[], unsigned int startByte);
    void parse_message(byte array[]);
    void hexToBytes(const char* code, byte* result);  
    void timeToString(char* string, size_t size);
    devicetype dt;

    bool softwareSerialUsed;
    SoftwareSerial mySoftwareSerial;
    bool inverted;
    bool done;
    byte buffer[90];
    byte message[123];    
    
    char m_cipherkey[33];
    byte m_blockCipherKey[16];

    uint32_t counter_reading_p_in;
    uint32_t counter_reading_p_out;
    uint32_t counter_reading_q_in;
    uint32_t counter_reading_q_out;
    uint32_t current_power_usage_in;
    uint32_t current_power_usage_out;    
};

#endif
