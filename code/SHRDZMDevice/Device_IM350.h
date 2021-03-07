#ifndef Device_IM350_H
#define Device_IM350_H

#define HDLC_FRAME_START_END 0x7E
#define REQUEST_TYPE_FRAME 2
#define GENERAL_GLO_CIPHERING 219
#define ENCRYPTION 32
#define BLOCK_SIZE 16

#include "DeviceBase.h"

class Device_IM350 : public DeviceBase
{   
  public:
    Device_IM350();
    ~Device_IM350();
    
    bool setDeviceParameter(JsonObject obj);
    bool initialize();    
    bool isNewDataAvailable();
    void prepare();
    SensorData* readParameterTypes();
    SensorData* readParameter();
    SensorData* readInitialSetupParameter();
    
  protected:
    void ResetData();
    char m_cipherkey[33];
    bool Translate(const char* code, const char *data);
    String getTimestamp();
    uint32_t getCounter_reading_p_in();
    uint32_t getCounter_reading_p_out();
    uint32_t getCounter_reading_q_in();
    uint32_t getCounter_reading_q_out();
    uint32_t getCurrent_power_usage_in();
    uint32_t getCurrent_power_usage_out();
  
    bool done;

    class Data
    {
      public:
        Data()
        {
          m_dataSize = 0;
          m_pData = NULL;
          m_pOutput = NULL;
          m_position = 0;
        };
        ~Data()
        {
          if(m_dataSize > 0)
          {
            free(m_pData);
            m_dataSize = 0;
            m_pData = NULL;
            m_pOutput = NULL;
          }
        }

        void ResetData()
        {
          if(m_pData != NULL)
          {
            free(m_pData);
            m_dataSize = 0;
            m_pData = NULL;
            m_pOutput = NULL;
          }          
        }
        
        byte getUInt8(int index);
        uint16_t getUInt16(int index);
        uint32_t getUInt32(int index);
        int getObjectCount();
        int getPosition();
        void set(byte *value, int count);
        void get(byte *target, int count);
        
        bool m_complete;
        byte *m_pData;
        byte *m_pOutput;
        int m_dataSize;
        int m_position;
    };
    
    class Cipher
    {
      public:
        class AV
        {
          public:
            AV()
            {
              for(int i = 0; i<16; i++)
                for(int j = 0; j<4; j++)
                  value[i][j] = 0;
            }
            uint32_t value[16][4];
        };

        Cipher();
        void ResetData();
        void setBlockCipherKey(byte *key);
        void generateWorkingkeys();
        void flushFinalBlock(Data *data);
        uint32_t toUInt32(byte* value, uint32_t offset);
        uint32_t subWord(uint32_t value);
        uint32_t shift(uint32_t value, uint32_t shift);
        uint32_t processBlock(byte *input_, uint32_t inOffset, byte *forOutput, uint32_t outOffset);
        void unPackBlock(byte *bytes_, uint32_t offset);
        void packBlock(byte *bytes_, uint32_t offset);
        void encryptBlock();
        void getUInt32(uint32_t value, byte *data, uint32_t offset);
        void initArray(byte *value);
        void getUint128_n(byte* buff, int pos, AV *reply);
        uint32_t bEToUInt32(byte* buff, uint32_t offset);
        void clone_n(AV *source, int pos, uint32_t *target);
        void multiplyP(uint32_t *data);
        void setArray(uint32_t *source, int pos, AV *target);
        void xor128_n(uint32_t *block, AV *value, int loc);
        void shiftRight(uint32_t * block, int count);
        void multiplyP8(uint32_t *x);
        void setJ0(byte *iv);
        void getGHash();
        void mxor(byte *block, byte *value);
        void multiplyH(byte *value);
        void uInt32ToBE(uint32_t value, byte *buff, int offset);
        void initC();
        void write(Data *data);
        int gCTRBlock(byte *buf, Data *data, int bufCount, int hhPos);
        
        int m_bytesRemaining, m_totalLength, m_blockSize;
        int m_position;
      protected:
        byte m_systemTitle[8];
        byte m_j0[16];
        byte m_counter[16];
        byte m_s[16];
        byte m_bufBlock[16];
        byte m_blockCipherKey[16];
        byte m_authenticationKey[16];
        int m_invocationCounter;
        uint32_t m_workingKey[11][4];
        uint32_t m_c0, m_c1, m_c2, m_c3;    
        int m_rounds;
        int m_hhPos;
        AV *m_pArray1;    // Array [32][16][4]        
    };


  private:
    bool dataAvailable;
    enum InterfaceType  
    {
      HDLC = 0,
      WRAPPER = 1,
      PDU = 2,
      WIRELESS_MBUS = 3,
    } m_interfaceType;

    byte LLC_REPLY_BYTES[3];

    void extractData();
    void decryptAesGcm(Data *data);
    void hexToBytes(const char* code, byte* result);
    byte getUInt8(byte *value, int index);
    uint16_t getUInt16(byte *data, int index);
    uint32_t getUInt32(byte *data, int index);
    void countFCS16(byte *buff, int offset, int count);
    int handleHDLC();
    void getDataFromFrame(byte *reply, InterfaceType hdlc);
    byte getHdlcData(byte *reply);
    bool checkHdlcAddress(byte *reply, int index, int *addresses);
    int getHDLCAddress(byte *buff);
    void dataSet(byte *value, int index, int count);
    void extractValues(Data *data);
    uint32_t byteToUInt32(byte array[], unsigned int startByte);
        
    byte m_blockCipherKey[16];
    int m_dataCapacity, m_dataPosition;
    byte m_authenticationKey[16];
    byte m_systemTitle[8];
    int m_invocationCounter;
    byte *m_pMessage;
    int m_pMessageLength;
    int m_packetLength;
    int m_position;
    Data m_data;
//    Data *m_data;
    Cipher m_cipher;
    uint16_t m_message_year;
    uint8_t m_message_month;
    uint8_t m_message_day;
    uint8_t m_message_hour;
    uint8_t m_message_minute;
    uint8_t m_message_second; 
    uint32_t m_counter_reading_p_in;
    uint32_t m_counter_reading_p_out;
    uint32_t m_counter_reading_q_in;
    uint32_t m_counter_reading_q_out;
    uint32_t m_current_power_usage_in;
    uint32_t m_current_power_usage_out;       
      
};

#endif
