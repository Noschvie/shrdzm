#ifndef Device_IM350_H
#define Device_IM350_H

#include "config/config.h"
#include "DeviceBase.h"

#include <Crypto.h>
#include <AES.h>
#include <GCM.h>
#include <SoftwareSerial.h>

struct Vector_GCM {
    const char *name;
    uint8_t key[16];
    uint8_t *ciphertextPos;
    uint8_t *ciphertextPos2;
    uint8_t authdata[16];
    uint8_t iv[12];
    uint8_t tag[12];
    size_t authsize;
    size_t datasize;
    size_t datasize2;
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
      e450,
      e450IRWien,
      am550,
      im350Wels,
      sagemcom,
      no,
      e450Steiermark,
      kaifaMBus1
    };
        
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:   
    bool wrongDebugSetupDetected;
    Vector_GCM Vector_SM;
    GCM<AES128> *gcmaes128 = 0;
    long baud = 115200;
    bool requestNeeded = false;
    
    void init_vector(Vector_GCM *vect, byte *key_SM, byte *readMessage, devicetype dt);
    void decrypt_text(Vector_GCM *vect, byte *bufferResult);
    uint16_t byteToUInt16(byte array[], unsigned int startByte);
    uint32_t byteToUInt32(byte array[], unsigned int startByte);
    bool readDataStream(bool withTimeout);
    void parse_message(byte array[], devicetype dt);
    void hexToBytes(const char* code, byte* result);  
    byte hexToByte(const char* code);
    String hexToString(byte array[], int readCnt);
    void timeToString(char* string, size_t size);
    uint32_t writeHourValue(const char *value);
    uint32_t writeDayValue(const char *value);
    time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss);

    bool softwareSerialUsed;
    SoftwareSerial mySoftwareSerial;
    bool inverted;
    bool done;
    int interval;
    byte *lastReadMessage;
    uint16_t lastReadMessageLen;
    bool cachedDataAvailable;
    
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
