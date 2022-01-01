#include "Device_IM350.h"

//#define SIMULATION
#define UART_RXD_INV (BIT(19)) 

const int messageLength = 123;
const byte firstByte = 0x7E; 
const byte lastByte = 0x7E; 
const int waitTime = 1500;


#ifdef SIMULATION

  // IM350
//  String c2 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92916FC1EF62AB5F476F8A59772745CC99365500ACF5EBEEA82F95581762C2D18804A1E7F7700FB10752F04D9344779C6A332C973EAF3CF375095D1821E87C68909EE47759AD925976C42E4D92FF9727E4213FDEE5F1ABE45D9D97F5E5E7E";
//  String c2 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92A57400A90D827A575C749DAB31F44F3C4B83D9B15F9FD20D9FE147F5AD3A0432A65208408AE177E31AF4CD985DAD92F7647BB5ED8F619F629059606BBC55128D5E9A23BB992F564D098BBE8E759ECC0A01B259A02599A236EC45475A07E";
//  String c1 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92A57400A90D827A575C749DAB31F44F3C4B83D9B15F9FD20D9FE147F5AD3A0432A65208408AE177E31AF4CD985DAD92F7647BB5ED8F619F629059606BBC55128D5E9A23BB992F564D098BBE8E759ECC0A01B259A02599A236EC45475A07E";  
//  String ckSimulation = "612888F59306A7855AC5C08CB3A96B39"; // IM350

  // AM550
//  String c1 = "7EA077CF022313BB45E6E700DB0849534B69745CF6C95F200000046BB45F8F43980519E16E36BC0C7198EB0E9CF1E0DF6A227D63C3F24852486F4A0CE8AA187991BF1C44544F62BBBB75BEEB4CDB549D7CF525CD95E6D82D2BE1C7F2ED84505402317F84AEB2A285508EC1B072553C39CD928F1F492941F67E0000";  
//  String c2 = "7EA077CF022313BB45E6E700DB0849534B69745CF6C95F200000046BB45F8F43980519E16E36BC0C7198EB0E9CF1E0DF6A227D63C3F24852486F4A0CE8AA187991BF1C44544F62BBBB75BEEB4CDB549D7CF525CD95E6D82D2BE1C7F2ED84505402317F84AEB2A285508EC1B072553C39CD928F1F492941F67E0000";  
  // Helmut (Wels)
  String c1 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000BE34111B482F835CF05AACA84AE6BEDD7EE0F17132CEC104495BD7E9C7F258775072AB713296B1E4CC9BE98468E24D194237365949209A5D6CA42584EB9BB7F5F7E50E5304886F3DA7A9342D157553B2E48A175D907E000000000000000000";  
  String c2 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000BE34111B482F835CF05AACA84AE6BEDD7EE0F17132CEC104495BD7E9C7F258775072AB713296B1E4CC9BE98468E24D194237365949209A5D6CA42584EB9BB7F5F7E50E5304886F3DA7A9342D157553B2E48A175D907E000000000000000000";  
//  String c2 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000C06514BA5DA79232B4036C892317BA7AAF692564DC5DDE0AC93A0997E52F44A5A1FC0505E81AE07CEBFB2845FB73D2D2B8608CE8DC9E6761AC970D3658CE754AA82421E1F05A7BE06D5CDE45995D3E7A596D896D317E000000000000000000";    
  String ckSimulation = "5A9AFEB2A17E6CF0CBAF98C8300ED9D7"; // Wels
  
  int number = 1;
#endif

Device_IM350::Device_IM350()
{    
  deviceTypeName = "IM350";
  dt = unknown;
  softwareSerialUsed = false;
  inverted = false;
  
  done = false;
  dataAvailable = false;  
  memset(m_cipherkey, 0, 33); 

  for(int i=0; i<16; i++)
  {
    m_blockCipherKey[i] = i;
    m_authenticationKey[i] = 208+i;
  }
  
  for(int i=0; i<8; i++)
  {
    m_systemTitle[i] = i+65;
  }
  
  m_invocationCounter = 0;
  m_pMessage = NULL;
  m_pMessageLength = 0;
  
  LLC_REPLY_BYTES[0] = 0xE6;
  LLC_REPLY_BYTES[1] = 0xE7;
  LLC_REPLY_BYTES[2] = 0x00;  

#ifdef SIMULATION
  //dt = am550;
  //dt = im350;
  dt = im350Wels;
#endif
}

Device_IM350::~Device_IM350()
{
  DLN(F("IM350 Instance deleted"));
}

bool Device_IM350::isNewDataAvailable()
{
  return dataAvailable;
}

bool Device_IM350::setDeviceParameter(JsonObject obj)
{ 
  inverted = false;
  
  if(obj.containsKey("requestpin"))
  {
    pinMode(atoi(obj["requestpin"]), OUTPUT);
    digitalWrite(atoi(obj["requestpin"]), LOW);   
  }
  if(obj.containsKey("cipherkey"))
  {
    String codeBuffer(obj["cipherkey"].as<char*>());
    codeBuffer.replace( " ", "" );
    
    if(codeBuffer.length() == 32)
    {
        memcpy(m_cipherkey, codeBuffer.c_str(), 32);

        obj["cipherkey"] = codeBuffer;
    }
    else
      return false;
  }  
  if(obj.containsKey("invertrx"))
  {
    String codeBuffer(obj["invertrx"].as<char*>());
    
    if(codeBuffer[0] == 'Y' || 
       codeBuffer[0] == 'y' ||
       codeBuffer[0] == '1' ||
       codeBuffer[0] == 'T' ||
       codeBuffer[0] == 't'
       )
    {
      obj["invertrx"] = "YES";
      inverted = true;
    }
    else
    {
      obj["invertrx"] = "NO";
    }    
  }  

  if(obj.containsKey("rxpin"))
  {
    if(obj["rxpin"].as<uint8_t>() != 3) // if not pin 3, software serial is needed
    {
      DLN(F("Device is running with SoftwareSerial"));

      mySoftwareSerial.begin(115200, SWSERIAL_8N1, obj["rxpin"].as<uint8_t>(), -1, inverted, 140);
      softwareSerialUsed = true;
    }
    else
    {
      if(inverted)
      {
        U0C0 = BIT(UCBN) | BIT(UCBN+1) | BIT(UCSBN); 
      }      
    }
  }  

  DeviceBase::setDeviceParameter(obj);
  
  return true;
}

void Device_IM350::prepare()
{
}

bool Device_IM350::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();
  deviceParameter["requestpin"] = "5";
  deviceParameter["cipherkey"] = "00000000000000000000000000000000";
  deviceParameter["rxpin"] = "3";
  deviceParameter["invertrx"] = "NO";
  deviceParameter["sendRawData"] = "NO";
  deviceParameter["autoRebootMinutes"] = "60";

  return true;
}

SensorData* Device_IM350::readParameterTypes()
{
  SensorData *al = new SensorData(6);

//  al->di[0].nameI = "encoded";
  al->di[0].nameI = "counter_reading_p_in";
  al->di[1].nameI = "counter_reading_p_out";
  al->di[2].nameI = "counter_reading_q_in";
  al->di[3].nameI = "counter_reading_q_out";
  al->di[4].nameI = "current_power_usage_in";
  al->di[5].nameI = "current_power_usage_out";

  return al;
}

SensorData* Device_IM350::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "120";

  return al; 
}

SensorData* Device_IM350::readParameter()
{  
  SensorData *al;

  ResetData();

  if(!deviceParameter["autoRebootMinutes"].isNull())
  {
    if(strcmp(deviceParameter["autoRebootMinutes"],"0") != 0)
    {
      // Check whether to reboot first
      if(millis() > (atol(deviceParameter["autoRebootMinutes"]) * 1000 * 60))
      {
        DLN(F("Will reboot now"));
        
        delay(500); 
        ESP.restart();      
      }
    }
  }
  else // Default-Reset every 60 minutes
  {
    if(millis() > 3600000)
    {
      DLN(F("Will reboot now"));
      
      delay(500); 
      ESP.restart();      
    }
  }


#ifdef SIMULATION
  String ck = ckSimulation;
#else
  String ck = deviceParameter["cipherkey"];
#endif

  if(ck == "00000000000000000000000000000000" || ck.length() != 32)
  {
    al = new SensorData(1);
    
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "cipherkey not set!";  
    
    return al;
  }

  digitalWrite(atoi(deviceParameter["requestpin"]), LOW);
  
  unsigned char incomingByte = 0;
  String code;
  bool dataWaitDone = false;
  bool dataError = false;
  byte message[messageLength];
  char hexCode[3];
  int cnt = 0;

  hexCode[2] = 0;

  memset(message, 0, messageLength);

#ifdef SIMULATION
  if(number == 1)
  {
    code = c1;
    number = 2;
  }
  else
  {
    code = c2;
    number = 1;
  }
#else

  if(softwareSerialUsed)
  {
    mySoftwareSerial.flush();

    while(mySoftwareSerial.available() > 0)
    {
      byte trash = mySoftwareSerial.read();
    }
  }
  else
  {
    Serial.flush();
    
    if(inverted)
      U0C0 = BIT(UCRXI) | BIT(UCBN) | BIT(UCBN+1) | BIT(UCSBN); 
  
    Serial.begin(115200);

    if(inverted)
      U0C0 = BIT(UCRXI) | BIT(UCBN) | BIT(UCBN+1) | BIT(UCSBN); 

    while(Serial.available() > 0)
    {
      byte trash = Serial.read();
    }
  }
   
  pinMode(LED_BUILTIN, OUTPUT); // LED als Output definieren

  // enable request
  digitalWrite(atoi(deviceParameter["requestpin"]), HIGH); 
  digitalWrite(LED_BUILTIN, LOW);

  delay(1000);
  unsigned long requestMillis = millis();

  if(softwareSerialUsed)
  {
    while ((mySoftwareSerial.available()) && (cnt < messageLength) && (millis()-requestMillis <= waitTime)) 
    {
      message[cnt] = mySoftwareSerial.read();
      if (message[0] != firstByte && cnt == 0) 
        continue;
      else 
        cnt++;
    }
  }
  else
  {
    while ((Serial.available()) && (cnt < messageLength) && (millis()-requestMillis <= waitTime)) 
    {
      message[cnt] = Serial.read();
      if (message[0] != firstByte && cnt == 0) 
        continue;
      else 
        cnt++;
    }
  }

  // disable request
  digitalWrite(atoi(deviceParameter["requestpin"]), LOW); 
  digitalWrite(LED_BUILTIN, HIGH);


  if(!softwareSerialUsed)
  {
    Serial.flush();
    Serial.begin(9600);
  }
  
  for(int i = 0; i<messageLength; i++)
  {
    sprintf(hexCode, "%02X", message[i]);
    code += String(hexCode);    
  }

  if (message[0] != firstByte)
  {
    if(!deviceParameter["sendRawData"].isNull() && strcmp(deviceParameter["sendRawData"],"YES") == 0)
    {
      al = new SensorData(2);
      al->di[1].nameI = "data";
      al->di[1].valueI = code;      
    }
    else
    {
      al = new SensorData(1);
    }
    
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "No supported SmartMeter Type identified - first Byte does not fit";  
        
    return al;
  }

  if(message[sizeof(message)-1] == lastByte)
    dt = im350;
   else if (message[sizeof(message)-3] == lastByte)
    dt = am550;
   else if (message[sizeof(message)-10] == lastByte)
    dt = im350Wels;
#endif

  if (dt == unknown)
  {
    if(!deviceParameter["sendRawData"].isNull() && strcmp(deviceParameter["sendRawData"],"YES") == 0)
    {    
      al = new SensorData(2);
      al->di[1].nameI = "data";
      al->di[1].valueI = code;
    }
    else
    {
      al = new SensorData(1);
    }
    
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "No supported SmartMeter Type identified - No end Byte found";  
            
    return al;
  }  
  else
  {    
    DLN( "SmartMeter Type = "+String(dt));  
  }

  if(code == "" || code == "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")
  {
    al = new SensorData(1);
    
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "no data read";  
    
//    Serial.flush();
//    Serial.begin(9600);
        
    return al;
  }

  if(dt == am550)
  {
    code = code.substring(0, code.length()-4);
  }
  else if(dt == im350Wels)
  {
    code = code.substring(0, code.length()-18);
  }

//  if(Translate(deviceParameter["cipherkey"].as<char*>(), code.c_str()))
  if(Translate(ck.c_str(), code.c_str()))
  {    
    if(!deviceParameter["sendRawData"].isNull() && strcmp(deviceParameter["sendRawData"],"YES") == 0)
    {
      al = new SensorData(7);
      al->di[6].nameI = "encoded";
      al->di[6].valueI = String(code.c_str());          
    }
    else
    {
      al = new SensorData(6);
    }

    al->di[0].nameI = "counter_reading_p_in";
    al->di[0].valueI = String(getCounter_reading_p_in());  

    al->di[1].nameI = "counter_reading_p_out";
    al->di[1].valueI = String(getCounter_reading_p_out());  

    al->di[2].nameI = "counter_reading_q_in";
    al->di[2].valueI = String(getCounter_reading_q_in());  
  
    al->di[3].nameI = "counter_reading_q_out";
    al->di[3].valueI = String(getCounter_reading_q_out());  

    al->di[4].nameI = "counter_power_usage_in";
    al->di[4].valueI = String(getCurrent_power_usage_in());  

    al->di[5].nameI = "counter_power_usage_out";
    al->di[5].valueI = String(getCurrent_power_usage_out()); 
  }
  else
  {
    if(!deviceParameter["sendRawData"].isNull() && strcmp(deviceParameter["sendRawData"],"YES") == 0)
    {
      al = new SensorData(2);
      al->di[1].nameI = "data";
      al->di[1].valueI = code;      
    }
    else
      al = new SensorData(1);
    
    
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "Translation of data not possible";      

  }

  Serial.flush();
  Serial.begin(9600);

  return al;  
}

void Device_IM350::ResetData()
{
 // m_data.ResetData(); 
}

////////////////////////////////
bool Device_IM350::Translate(const char* code, const char *data)
{    
  String codeBuffer(code);
  codeBuffer.replace( " ", "" );

  DV(code);
  
  m_position = 0;
  m_cipher.ResetData();
  
  if(codeBuffer.length() != 32)
    return false;

  hexToBytes(code, m_blockCipherKey);
  m_cipher.setBlockCipherKey(m_blockCipherKey);
  
  if(m_pMessage != NULL)
  {
    free(m_pMessage);
    m_pMessage = NULL;
  }
  String dataBuffer(data);
  dataBuffer.replace( " ", "" );
  
  if((dataBuffer.length() % 2) != 0)
    return false;

  m_pMessageLength = dataBuffer.length() / 2;

  m_pMessage = (byte *)malloc(m_pMessageLength);
  hexToBytes(data, m_pMessage); 
  
  if(getUInt8(m_pMessage, 0) == 0x7e)
  {
    m_interfaceType = InterfaceType::HDLC;
    int frame = handleHDLC();

    getDataFromFrame(m_pMessage, m_interfaceType);
        
    extractData();
  }
  else
  {
    // not implemented
    DLN(F("HDLC not implemented!"));
    return false;
  }
  
  return true;
}

String Device_IM350::getTimestamp()
{
  char reply[20];
  reply[19] = 0;
  sprintf (reply, "%02d-%02d-%02d %02d:%02d:%02d", m_message_year, m_message_month, m_message_day, m_message_hour, m_message_minute, m_message_second);
  
  return String(reply);
}

uint32_t Device_IM350::getCounter_reading_p_in()
{
  return m_counter_reading_p_in;
}

uint32_t Device_IM350::getCounter_reading_p_out()
{
  return m_counter_reading_p_out;
}

uint32_t Device_IM350::getCounter_reading_q_in()
{
  return m_counter_reading_q_in;
}

uint32_t Device_IM350::getCounter_reading_q_out()
{
  return m_counter_reading_q_out;
}

uint32_t Device_IM350::getCurrent_power_usage_in()
{
  return m_current_power_usage_in;
}

uint32_t Device_IM350::getCurrent_power_usage_out()
{
  return m_current_power_usage_out;
}
    
void Device_IM350::extractData()
{
  byte cmd = m_data.getUInt8(-1);


  if(cmd == GENERAL_GLO_CIPHERING)
  {
    int originalPosition = m_data.getPosition();

    Data d;
    d.set(m_data.m_pData, m_data.m_dataSize);

    decryptAesGcm(&d);
  }
  else
  {
    DLN("no GENERAL_GLO_CIPHERING");    
    DV(m_data.m_position);
  }
}

void Device_IM350::decryptAesGcm(Data *data)
{
  byte cmd = data->getUInt8(-1);
  
  if(cmd == GENERAL_GLO_CIPHERING)
  {
    int len_title = data->getObjectCount(); // title

    byte title[len_title];    
    data->get(title, len_title);
            
    int len_ = data->getObjectCount(); // ciphered content len
    byte cipheredContent[data->m_dataSize-data->m_position];    
    memcpy(cipheredContent, data->m_pData+(data->m_position), data->m_dataSize-data->m_position);

    
    byte security = data->getUInt8(-1) & 0x30;
    
    uint32_t invocationCounter = data->getUInt32(-1);

    
    if(security != ENCRYPTION)
    {
      DLN(F("No Encryption!"));
      return;
    }

    len_ = data->m_dataSize - data->m_position;
    len_ = 90;

    byte ciphertext[len_];
    data->get(ciphertext, len_);
  
    Data aad;
    if(dt == im350)
       aad.set(m_pMessage+30, len_);
    else if(dt == am550)
      aad.set(m_pMessage+28, len_);
    else if(dt == im350Wels)
//      aad.set(m_pMessage+30, len_);
      aad.set(m_pMessage+30, len_-9);
    else
    {
      DLN(F("Decryption Type not implemented!"));
      return;    
    }

    byte iv[13];
    memcpy(iv, title, 8);
    iv[8] = ((invocationCounter >> 24) & 0xFF);
    iv[9] = ((invocationCounter >> 16) & 0xFF);
        iv[10] = ((invocationCounter >> 8) & 0xFF);
        iv[11] = (invocationCounter & 0xFF);
        iv[12] = 0;

    byte h[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte hr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    m_cipher.processBlock(h, 0, hr, 0);   
    m_cipher.setJ0(iv);   
    m_cipher.initArray(hr);   
      
    m_cipher.getGHash();

    m_cipher.initC();
    m_cipher.write(&aad);

    m_cipher.flushFinalBlock(&aad);
    
    // output
 //   Serial.println("decrypted = ");
    char buffer[3];
    buffer[2] = 0;
    for(int i = 0; i<aad.m_dataSize; i++) 
    {
//      sprintf (buffer, "0x%02X,%d", aad.m_pOutput[i],aad.m_pOutput[i]);
      sprintf (buffer, "%02X", aad.m_pOutput[i]);
      Serial.print(buffer);
      Serial.print(" ");
    }
    Serial.println();
    
    // extract data
    extractValues(&aad);
  }
  else
  {
    Serial.println("No GENERAL_GLO_CIPHERING!");
  }
}

void Device_IM350::extractValues(Data *data)
{
  m_message_year = (data->m_pOutput[6] << 8) + (data->m_pOutput[7]);
  m_message_month = data->m_pOutput[8];
  m_message_day = data->m_pOutput[9];
  m_message_hour = data->m_pOutput[11];
  m_message_minute = data->m_pOutput[12];
  m_message_second = data->m_pOutput[13];
  
  m_counter_reading_p_in = byteToUInt32(data->m_pOutput, 57);
  m_counter_reading_p_out = byteToUInt32(data->m_pOutput, 62);
  m_counter_reading_q_in = byteToUInt32(data->m_pOutput, 67);
  m_counter_reading_q_out = byteToUInt32(data->m_pOutput, 72);
  m_current_power_usage_in = byteToUInt32(data->m_pOutput, 77);
  m_current_power_usage_out = byteToUInt32(data->m_pOutput, 82);  
}

uint32_t Device_IM350::byteToUInt32(byte array[], unsigned int startByte)
{
    uint32_t result;
    result = (uint32_t) array[startByte] << 24;
    result |=  (uint32_t) array[startByte+1] << 16;
    result |= (uint32_t) array[startByte+2] << 8;
    result |= (uint32_t) array[startByte+3];

    return result;
}

void Device_IM350::getDataFromFrame(byte *reply, InterfaceType hdlc)
{
  int offset = 0;
  int cnt = m_packetLength - m_position;
  
  if (cnt != 0)
  {
    m_dataCapacity = offset + cnt;
    
    dataSet(reply, m_position, cnt);
    m_position = m_position + 3;
  }
  m_dataPosition = offset;
}

void Device_IM350::dataSet(byte *value, int index = -1, int count = -1)
{
  if (index == -1)
  {
    index = 0;
  }
  
  if (count == -1)
  {
    count = m_pMessageLength - index;
  }

  if(count != 0)
  {
    m_data.m_dataSize = count;

    if(m_data.m_pData != NULL)
      free(m_data.m_pData);
      
    m_data.m_position = 0;
    m_data.m_pData = (byte *)malloc(m_data.m_dataSize);
    
    memcpy(m_data.m_pData, value+index, count-index);       
  }
}

int Device_IM350::handleHDLC()
{
  return getHdlcData(m_pMessage);
}

void Device_IM350::hexToBytes(const char* code, byte* result)
{
  if(code != NULL)
  {
    String buffer(code);
    char hexCode[2];
    
    buffer.replace( " ", "" );
    
    for(int i = 0; i<buffer.length() / 2; i++)
    {
      result[i] = strtoul(buffer.substring(i*2, i*2+2).c_str(), NULL, 16);
    }   
  } 
}

byte Device_IM350::getUInt8(byte *data, int index = -1)
{
  byte value = 0;
  
  if (index == -1)
  {
    index = m_position;
    value = data[index] & 0xFF;
    value = value % (byte)pow(2,8); 
    m_position += 1;
    return value;
  }

  value = data[index] & 0xFF; 
  value = value % (byte)pow(2,8); // !?
  
  return value;
}

uint16_t Device_IM350::getUInt16(byte *data, int index = -1)
{
  uint16_t value = 0;
  
  if (index == -1)
  {
    index = m_position;
    value = ((data[index] & 0xFF) << 8) | (data[index + 1] & 0xFF);
    value = value % (uint16_t)pow(2, 16);
    m_position += 2;
    return value;
  }
  
  value = ((data[index] & 0xFF) << 8) | (data[index + 1] & 0xFF);
  value = value % (uint16_t)pow(2, 16);
  return value;
}

uint32_t Device_IM350::getUInt32(byte *data, int index = -1)
{
  uint32_t value = 0;
  
  if (index == -1)
  {
    index = m_position;
    m_position += 4;
  }
  
  value = data[index] & 0xFF;
  value = value << 24;
  value |= (data[index + 1] & 0xFF) << 16;
  value |= (data[index + 2] & 0xFF) << 8;
  value |= (data[index + 3] & 0xFF);
  value = value % (uint32_t) pow(2, 32);
  return value;
}

void Device_IM350::countFCS16(byte *buff, int offset, int count)
{
  Serial.println("Da fehlt noch was");
}
    
byte Device_IM350::getHdlcData(byte *reply)
{
  byte ch = 0;
  byte cf = 0;
  bool server = true; // only if getHdlcData is used 1 time!!!!
  int pos = m_position;
  int packetStartID = m_position;
  int frameLen = 0;
  int crc = 0;
  int crcRead = 0;

  if (m_pMessageLength - m_position < 9)
  {
    m_data.m_complete = false;
    return 0;
  }
  
  m_data.m_complete = true;
  
  while (pos < m_pMessageLength)
  {
    ch = getUInt8(reply);

    if (ch == HDLC_FRAME_START_END)
    {
      packetStartID = pos;
      break;
    }
    pos += 1;
  }
  
  if (m_position == m_pMessageLength)
  {
    m_data.m_complete = false;
    return 0;
  }
  
  int frame_ = getUInt8(reply);
  
  if ((frame_ & 0xF0) != 0xA0)
  {
    m_position = m_position - 1;

    return getHdlcData(reply);
  }
  
  if ((frame_ & 0x7) != 0)
  {
    frameLen = ((frame_ & 0x7) << 8);
  }
  
  ch = getUInt8(reply);
  
  frameLen += ch;
  
  if (m_pMessageLength - m_position + 1 < frameLen)
  {
    m_data.m_complete = false;
    return 0;
  }
  
  int eopPos = frameLen + packetStartID + 1;
  ch = getUInt8(reply, eopPos);

  if (ch != HDLC_FRAME_START_END)
  {
    m_position = m_position - 2;
    return getHdlcData(reply);
  }
  
  int addresses[] = {0,0};

  bool ret = checkHdlcAddress(reply, eopPos, addresses);
  
  cf = getUInt8(reply);
  
  if ((frame_ & 0x8) != 0)
  {
    Serial.println( "TODO (frame_ & 0x8)");
  }
  else
  {
  }
  
  crcRead = getUInt16(reply);
  
  if (m_position != packetStartID + frameLen + 1)
  {
    crcRead = getUInt16(reply, packetStartID + frameLen - 1);
    m_packetLength = (eopPos - 2);
  }
  else
  {
    m_packetLength = (m_position + 1);
  }
  
  if (m_position == packetStartID + frameLen + 1)
  {
    getUInt8(reply); // will never be reached!
  }
  else
  {
    // check next 3 bytes against LLC_REPLY_BYTES
    if( getUInt8(reply) != LLC_REPLY_BYTES[0] ||
      getUInt8(reply) != LLC_REPLY_BYTES[1] ||
      getUInt8(reply) != LLC_REPLY_BYTES[2])
    {
      Serial.println("Error with LLC_REPLY_BYTES!");
    }
  }
        
    return cf; 
}

  
int Device_IM350::getHDLCAddress(byte *buff)
{
  int size = 0;
  int ret = 0;
  int pos = m_position;
  while (pos != m_pMessageLength)
  {
    size += 1;
    if (getUInt8(buff, pos) & 0x1 == 1)
      break;
    pos += 1;
  }
  if (size == 1)
  {
    ret = (getUInt8(buff) & 0xFE) >> 1;
  }
  else if (size == 2)
  {
    ret = getUInt16(buff);
    ret = ((ret & 0xFE) >> 1) | ((ret & 0xFE00) >> 2);
  }
  else if (size == 4)
  {
    ret = getUInt32(buff);
    ret = ((ret & 0xFE) >> 1) | ((ret & 0xFE00) >> 2) | ((ret & 0xFE0000) >> 3) | ((ret & 0xFE000000) >> 4);
  }
  else
  {
    Serial.println("Wrong size.");
  }
  return ret;
}

bool Device_IM350::checkHdlcAddress(byte *reply, int index, int *addresses)
{
  addresses[0] = getHDLCAddress(reply);
  addresses[1] = getHDLCAddress(reply);
  
  return true;
}

Device_IM350::Cipher::Cipher()
{
  for(int i = 0; i<16; i++)
  {
    m_authenticationKey[i] = 208+i;
    m_s[i] = 0;
  }
  
  m_invocationCounter = 0;
  m_rounds = 0;
  m_bytesRemaining = 0;
  m_totalLength = 0;
  m_blockSize = 16;
  
  for(int i = 0; i<8; i++)
    m_systemTitle[i] = i+65;
  
  m_position = 0;
}

void Device_IM350::Cipher::ResetData()
{
  free(m_pArray1);
  
  for(int i = 0; i<16; i++)
  {
    m_authenticationKey[i] = 208+i;
    m_s[i] = 0;
  }
  
  m_invocationCounter = 0;
  m_rounds = 0;
  m_bytesRemaining = 0;
  m_totalLength = 0;
  m_blockSize = 16;
  
  for(int i = 0; i<8; i++)
    m_systemTitle[i] = i+65;
  
  m_position = 0;  
}

int Device_IM350::Cipher::gCTRBlock(byte *buf, Data *data, int bufCount, int hhPos)
{
  int i = 15;
  while (i >= 12)
  {
    m_counter[i] += 1;
    if (m_counter[i] != 0)
      break;
    i -= 1;
  }
  
  byte tmp[BLOCK_SIZE];
  byte hashBytes[BLOCK_SIZE];
  
  processBlock(m_counter, 0, tmp, 0);

  for(int i = bufCount; i< BLOCK_SIZE; i++)
    tmp[i] = 0;
  
  memcpy( hashBytes, tmp, BLOCK_SIZE );
  int pos = 0;

  
  while (pos != bufCount)
  {
    tmp[pos] ^= buf[pos];
    
    data->m_pOutput[hhPos] = tmp[pos];

    pos += 1;
    hhPos++;
  }
  
  mxor(m_s, hashBytes);
  multiplyH(m_s);
  
  return hhPos;
}

void Device_IM350::Cipher::flushFinalBlock(Data *data)
{  
  if (m_bytesRemaining > 0)
  {   
    byte tmp[m_bytesRemaining];
    memcpy(tmp, m_bufBlock, m_bytesRemaining);
  
    gCTRBlock(tmp, data, m_bytesRemaining, 80);
  }
}

void Device_IM350::Cipher::write(Data *data)
{
  m_bytesRemaining = 0;
  
  for(int i=0; i<16; i++)
    m_bufBlock[i] = 0;
  
  int m_hhPos = 0;

  for(int i = 0; i<data->m_dataSize; i++)
  {
    m_bufBlock[m_bytesRemaining] = data->m_pData[i];

    m_bytesRemaining += 1;    
      
    if (m_bytesRemaining == BLOCK_SIZE)
    {     
      m_hhPos = gCTRBlock(m_bufBlock, data, BLOCK_SIZE, m_hhPos);
            
      m_bytesRemaining = 0;
      m_position += BLOCK_SIZE;
    }     
  }
}

void Device_IM350::Cipher::initC()
{
  m_c0 = 0;
  m_c1 = 0;
  m_c2 = 0;
  m_c3 = 0;
}

void Device_IM350::Cipher::setJ0(byte *iv)
{
  memset(m_counter, 0 , 16);
  memset(m_j0, 0 , 16);
  for(int i=0; i<12; i++)
    m_j0[i] = iv[i];
  
  m_j0[15] = 0x01;
  
  memcpy(m_counter, m_j0, 16);
}

uint32_t Device_IM350::Cipher::bEToUInt32(byte* buff, uint32_t offset)
{
  uint32_t value = (buff[offset] << 24);
  
  value |= (buff[offset + 1] << 16) & 0xFF0000;
  value |= (buff[offset + 2] << 8) & 0xFF00;
  value |= buff[offset + 3] & 0xFF;
  
  return value;
}

void Device_IM350::Cipher::getUint128_n(byte* buff, int pos, AV *reply)
{
  reply->value[pos][0] = bEToUInt32(buff, 0);
  reply->value[pos][1] = bEToUInt32(buff, 4);
  reply->value[pos][2] = bEToUInt32(buff, 8);
  reply->value[pos][3] = bEToUInt32(buff, 12);
}
  
void Device_IM350::Cipher::clone_n(AV *source, int pos, uint32_t *target)
{
  target[0] = source->value[pos][0];
  target[1] = source->value[pos][1];
  target[2] = source->value[pos][2];
  target[3] = source->value[pos][3];
}

void Device_IM350::Cipher::multiplyP(uint32_t *data)
{
  bool lsb = (data[3] & 1) != 0;
  shiftRight(data, 1);
  if (lsb)
    data[0] ^= 0xe1000000;  
}
  
void Device_IM350::Cipher::shiftRight(uint32_t * block, int count)
{
  int bit = 0;
  int i = 0;
  uint32_t b;
  
  while (i < 4)
  {
    b = block[i];
    block[i] = (b >> count) | bit;
    bit = (b << (32 - count)) & 0xFFFFFFFF;
    i += 1;
  }
}
    
void Device_IM350::Cipher::setArray(uint32_t *source, int pos, AV *target)
{
  target->value[pos][0] = source[0];
  target->value[pos][1] = source[1];
  target->value[pos][2] = source[2];
  target->value[pos][3] = source[3];
}
      
void Device_IM350::Cipher::xor128_n(uint32_t *block, AV *value, int loc)
{
  int pos = 0;
  while (pos != 4)
  {
    block[pos] ^= value->value[loc][pos];
    pos += 1;
  }
}

void Device_IM350::Cipher::multiplyP8(uint32_t *x)
{
  uint32_t lsw = x[3];
  shiftRight(x, 8);
  int pos = 0;
  while (pos != 8)
  {
    if ((lsw & (1 << pos)) != 0)
      x[0] ^= ((0xe1000000 >> (7 - pos)) & 0xFFFFFFFF);
    pos += 1;
  }
}

void Device_IM350::Cipher::getGHash()
{
  int pos = 0;
  
  while (pos < 16)
  {
    byte x[16];
    uint32_t cnt = min(16 - pos, 16);
    
    for(int i = 0; i< cnt; i++)
      x[i] = m_authenticationKey[pos+i];

    for(int i = 0; i< 16; i++)
      m_s[i] = 0;
      
    mxor(m_s, x);   
    
    multiplyH(m_s);

    pos += 16;
  }
}
  
void Device_IM350::Cipher::uInt32ToBE(uint32_t value, byte *buff, int offset)
{
  buff[offset] = (value >> 24) & 0xFF;
  buff[offset + 1] = (value >> 16) & 0xFF;
  buff[offset + 2] = (value >> 8) & 0xFF;
  buff[offset + 3] = value & 0xFF;
}
  
void Device_IM350::Cipher::multiplyH(byte *value)
{
  uint32_t tmp[] = {0,0,0,0};
  uint32_t m[4];
  int pos = 0;
  while (pos != 16)
  {
    clone_n(m_pArray1+(pos + pos), (value[pos] & 0x0f), m);
    tmp[0] ^= m[0];
    tmp[1] ^= m[1];
    tmp[2] ^= m[2];
    tmp[3] ^= m[3];
    clone_n(m_pArray1+(pos + pos + 1), (value[pos] & 0xf0) >> 4, m);
    tmp[0] ^= m[0];
    tmp[1] ^= m[1];
    tmp[2] ^= m[2];
    tmp[3] ^= m[3];
    pos += 1;
  }
  uInt32ToBE(tmp[0], value, 0);
  uInt32ToBE(tmp[1], value, 4);
  uInt32ToBE(tmp[2], value, 8);
  uInt32ToBE(tmp[3], value, 12);
} 
  
void Device_IM350::Cipher::mxor(byte *block, byte *value)
{
  int pos = 0;
  while (pos != 16)
  {
    block[pos] ^= value[pos];
    pos += 1;
  }
}   
    
void Device_IM350::Cipher::initArray(byte *value)
{
  m_pArray1 = (AV*)calloc(32, sizeof(AV));
  
  getUint128_n(value, 8, m_pArray1+1);
  
  int pos = 4;
  uint32_t tmp[4];  
  int a = 1, b = 8;
  
  while (pos >= 1)
  {
    clone_n(m_pArray1+1, pos+pos, tmp);
    multiplyP(tmp);   
    setArray(tmp, pos, m_pArray1+1);    
    pos >>= 1;
  }

  a = 1;
  b = 1;
  
  clone_n(m_pArray1+a, b, tmp);
  multiplyP(tmp);

  a= 0;
  b= 8;
  setArray(tmp, b, m_pArray1);

  pos = 4;
  while (pos >= 1)
  {
    clone_n(m_pArray1, (pos+pos), tmp);
    multiplyP(tmp); 
    setArray(tmp, pos, m_pArray1);
    pos >>= 1;
  } 

  int pos1 = 0;
  int pos2;
  int k;

  while (true)
  {
    pos2 = 2;
    while (pos2 < 16)
    {
      k = 1;
      while (k < pos2)
      {
        clone_n(m_pArray1+pos1, pos2, tmp);
        xor128_n(tmp, m_pArray1+pos1, k);
        setArray(tmp, pos2+k, m_pArray1+pos1);
        k+=1;
      }
      pos2 += pos2;
    }
    pos1 += 1;
    if(pos1 == 32)
      return;
    
    if (pos1 > 1)
    {
      for(int i = 0; i< 16; i++)
        for(int j = 0; j<4; j++)
          m_pArray1[pos1].value[i][j] = 0;
        
      pos = 8;
      while (pos > 0)
      {
        clone_n(m_pArray1+(pos1 - 2), pos, tmp);
        multiplyP8(tmp);
        setArray(tmp, pos, m_pArray1+pos1);
        pos >>= 1;
      }
    }       
  }
  
  return;

  // !!!!!
  //free(m_pArray); 
}

uint32_t Device_IM350::Cipher::processBlock(byte *input_, uint32_t inOffset, byte *forOutput, uint32_t outOffset)
{
  unPackBlock(input_, inOffset);
  encryptBlock();
  packBlock(forOutput, outOffset);
  
  return BLOCK_SIZE;
}

void Device_IM350::Cipher::packBlock(byte *bytes_, uint32_t offset)
{ 
  getUInt32(m_c0, bytes_, offset);
  getUInt32(m_c1, bytes_, offset + 4);
  getUInt32(m_c2, bytes_, offset + 8);
  getUInt32(m_c3, bytes_, offset + 12);
} 

void Device_IM350::Cipher::getUInt32(uint32_t value, byte *data, uint32_t offset)
{
  data[offset] = value & 0xFF;
  data[offset + 1] = (value >> 8) & 0xFF;
  data[offset + 2] = (value >> 16) & 0xFF;
  data[offset + 3] = (value >> 24) & 0xFF;
}

void Device_IM350::Cipher::setBlockCipherKey(byte *key)
{
  memcpy(m_blockCipherKey, key, 16);
  
  generateWorkingkeys();
}

uint32_t Device_IM350::Cipher::toUInt32(byte* value, uint32_t offset)
{
  uint32_t tmp;
  
  tmp = value[offset] & 0xFF;
  tmp |= (value[offset + 1] << 8) & 0xFF00;
  tmp |= (value[offset + 2] << 16) & 0xFF0000;
  tmp |= (value[offset + 3] << 24) & 0xFF000000;
  
  return tmp;
}

uint32_t Device_IM350::Cipher::shift(uint32_t value, uint32_t shift)
{
    return (value >> shift) | (value << (32 - shift)) & 0xFFFFFFFF;
}

uint32_t Device_IM350::Cipher::subWord(uint32_t value)
{
    uint8_t S_BOX[] = {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,\
        0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76, 0xCA, 0x82,\
        0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF,\
        0x9C, 0xA4, 0x72, 0xC0, 0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F,\
        0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,\
        0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12,\
        0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75, 0x09, 0x83, 0x2C, 0x1A,\
        0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3,\
        0x2F, 0x84, 0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,\
        0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF, 0xD0, 0xEF,\
        0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F,\
        0x50, 0x3C, 0x9F, 0xA8, 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D,\
        0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,\
        0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7,\
        0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73, 0x60, 0x81, 0x4F, 0xDC,\
        0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E,\
        0x0B, 0xDB, 0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,\
        0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79, 0xE7, 0xC8,\
        0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA,\
        0x65, 0x7A, 0xAE, 0x08, 0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6,\
        0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,\
        0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35,\
        0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E, 0xE1, 0xF8, 0x98, 0x11,\
        0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55,\
        0x28, 0xDF, 0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,\
    0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16};
  
  uint32_t tmp = S_BOX[value & 0xFF] & 0xFF;
  tmp |= (((S_BOX[(value >> 8) & 0xFF]) & 0xFF) << 8) & 0xFF00;
  tmp |= (((S_BOX[(value >> 16) & 0xFF]) & 0xFF) << 16) & 0xFF0000;
  tmp |= (((S_BOX[(value >> 24) & 0xFF]) & 0xFF) << 24) & 0xFF000000;
  return tmp;
}

void Device_IM350::Cipher::generateWorkingkeys()
{
  int keyLen = int(sizeof(m_blockCipherKey) / 4);
  m_rounds = keyLen + 6;
  
  for(int a = 0; a<11; a++)
    for(int b = 0; b<4; b++)
      m_workingKey[a][b] = 0;
  
  uint8_t R_CON[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,\
        0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc,\
        0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,\
    0xc5, 0x91};
    
  uint32_t t = 0;
  uint32_t i = 0;
  uint32_t temp;
  
  while (i < sizeof(m_blockCipherKey))
  {
        m_workingKey[t >> 2][t & 3] = toUInt32(m_blockCipherKey, i);

        i += 4;
        t += 1;
  }

  int k = (m_rounds + 1) << 2;
  i = keyLen;
  
  while (i < k)
  {
    temp = m_workingKey[(i - 1) >> 2][(i - 1) & 3];
      
    if ((i % keyLen) == 0)
    {
      temp = subWord(shift(temp, 8)) ^ (R_CON[int(i / keyLen) - 1] & 0xFF);
    }
    else if ((keyLen > 6) and ((i % keyLen) == 4))
      temp = subWord(temp);
    m_workingKey[i >> 2][i & 3] = m_workingKey[(i - keyLen) >> 2][(i - keyLen) & 3] ^ temp;

    i += 1;
  }
  
/*  for(int i = 0; i< 11; i++)
  {
    for(int j = 0; j< 4; j++)
    {
      Serial.print(m_workingKey[i][j]);
      Serial.print(":");
    }
    Serial.println();
  } */

}

void Device_IM350::Cipher::unPackBlock(byte *bytes_, uint32_t offset)
{ 
  m_c0 = toUInt32(bytes_, offset);
  m_c1 = toUInt32(bytes_, offset + 4);
  m_c2 = toUInt32(bytes_, offset + 8);
  m_c3 = toUInt32(bytes_, offset + 12); 
}

void Device_IM350::Cipher::encryptBlock()
{   
    uint8_t S_BOX[] = {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,\
        0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76, 0xCA, 0x82,\
        0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF,\
        0x9C, 0xA4, 0x72, 0xC0, 0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F,\
        0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,\
        0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12,\
        0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75, 0x09, 0x83, 0x2C, 0x1A,\
        0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3,\
        0x2F, 0x84, 0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,\
        0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF, 0xD0, 0xEF,\
        0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F,\
        0x50, 0x3C, 0x9F, 0xA8, 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D,\
        0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,\
        0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7,\
        0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73, 0x60, 0x81, 0x4F, 0xDC,\
        0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E,\
        0x0B, 0xDB, 0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,\
        0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79, 0xE7, 0xC8,\
        0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA,\
        0x65, 0x7A, 0xAE, 0x08, 0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6,\
        0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,\
        0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35,\
        0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E, 0xE1, 0xF8, 0x98, 0x11,\
        0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55,\
        0x28, 0xDF, 0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,\
    0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16};
    
    uint32_t AES[] = {0xa56363c6, 0x847c7cf8, 0x997777ee,\
        0x8d7b7bf6, 0x0df2f2ff, 0xbd6b6bd6, 0xb16f6fde, 0x54c5c591,\
        0x50303060, 0x03010102, 0xa96767ce, 0x7d2b2b56, 0x19fefee7,\
        0x62d7d7b5, 0xe6abab4d, 0x9a7676ec, 0x45caca8f, 0x9d82821f,\
        0x40c9c989, 0x877d7dfa, 0x15fafaef, 0xeb5959b2, 0xc947478e,\
        0x0bf0f0fb, 0xecadad41, 0x67d4d4b3, 0xfda2a25f, 0xeaafaf45,\
        0xbf9c9c23, 0xf7a4a453, 0x967272e4, 0x5bc0c09b, 0xc2b7b775,\
        0x1cfdfde1, 0xae93933d, 0x6a26264c, 0x5a36366c, 0x413f3f7e,\
        0x02f7f7f5, 0x4fcccc83, 0x5c343468, 0xf4a5a551, 0x34e5e5d1,\
        0x08f1f1f9, 0x937171e2, 0x73d8d8ab, 0x53313162, 0x3f15152a,\
        0x0c040408, 0x52c7c795, 0x65232346, 0x5ec3c39d, 0x28181830,\
        0xa1969637, 0x0f05050a, 0xb59a9a2f, 0x0907070e, 0x36121224,\
        0x9b80801b, 0x3de2e2df, 0x26ebebcd, 0x6927274e, 0xcdb2b27f,\
        0x9f7575ea, 0x1b090912, 0x9e83831d, 0x742c2c58, 0x2e1a1a34,\
        0x2d1b1b36, 0xb26e6edc, 0xee5a5ab4, 0xfba0a05b, 0xf65252a4,\
        0x4d3b3b76, 0x61d6d6b7, 0xceb3b37d, 0x7b292952, 0x3ee3e3dd,\
        0x712f2f5e, 0x97848413, 0xf55353a6, 0x68d1d1b9, 0x00000000,\
        0x2cededc1, 0x60202040, 0x1ffcfce3, 0xc8b1b179, 0xed5b5bb6,\
        0xbe6a6ad4, 0x46cbcb8d, 0xd9bebe67, 0x4b393972, 0xde4a4a94,\
        0xd44c4c98, 0xe85858b0, 0x4acfcf85, 0x6bd0d0bb, 0x2aefefc5,\
        0xe5aaaa4f, 0x16fbfbed, 0xc5434386, 0xd74d4d9a, 0x55333366,\
        0x94858511, 0xcf45458a, 0x10f9f9e9, 0x06020204, 0x817f7ffe,\
        0xf05050a0, 0x443c3c78, 0xba9f9f25, 0xe3a8a84b, 0xf35151a2,\
        0xfea3a35d, 0xc0404080, 0x8a8f8f05, 0xad92923f, 0xbc9d9d21,\
        0x48383870, 0x04f5f5f1, 0xdfbcbc63, 0xc1b6b677, 0x75dadaaf,\
        0x63212142, 0x30101020, 0x1affffe5, 0x0ef3f3fd, 0x6dd2d2bf,\
        0x4ccdcd81, 0x140c0c18, 0x35131326, 0x2fececc3, 0xe15f5fbe,\
        0xa2979735, 0xcc444488, 0x3917172e, 0x57c4c493, 0xf2a7a755,\
        0x827e7efc, 0x473d3d7a, 0xac6464c8, 0xe75d5dba, 0x2b191932,\
        0x957373e6, 0xa06060c0, 0x98818119, 0xd14f4f9e, 0x7fdcdca3,\
        0x66222244, 0x7e2a2a54, 0xab90903b, 0x8388880b, 0xca46468c,\
        0x29eeeec7, 0xd3b8b86b, 0x3c141428, 0x79dedea7, 0xe25e5ebc,\
        0x1d0b0b16, 0x76dbdbad, 0x3be0e0db, 0x56323264, 0x4e3a3a74,\
        0x1e0a0a14, 0xdb494992, 0x0a06060c, 0x6c242448, 0xe45c5cb8,\
        0x5dc2c29f, 0x6ed3d3bd, 0xefacac43, 0xa66262c4, 0xa8919139,\
        0xa4959531, 0x37e4e4d3, 0x8b7979f2, 0x32e7e7d5, 0x43c8c88b,\
        0x5937376e, 0xb76d6dda, 0x8c8d8d01, 0x64d5d5b1, 0xd24e4e9c,\
        0xe0a9a949, 0xb46c6cd8, 0xfa5656ac, 0x07f4f4f3, 0x25eaeacf,\
        0xaf6565ca, 0x8e7a7af4, 0xe9aeae47, 0x18080810, 0xd5baba6f,\
        0x887878f0, 0x6f25254a, 0x722e2e5c, 0x241c1c38, 0xf1a6a657,\
        0xc7b4b473, 0x51c6c697, 0x23e8e8cb, 0x7cdddda1, 0x9c7474e8,\
        0x211f1f3e, 0xdd4b4b96, 0xdcbdbd61, 0x868b8b0d, 0x858a8a0f,\
        0x907070e0, 0x423e3e7c, 0xc4b5b571, 0xaa6666cc, 0xd8484890,\
        0x05030306, 0x01f6f6f7, 0x120e0e1c, 0xa36161c2, 0x5f35356a,\
        0xf95757ae, 0xd0b9b969, 0x91868617, 0x58c1c199, 0x271d1d3a,\
        0xb99e9e27, 0x38e1e1d9, 0x13f8f8eb, 0xb398982b, 0x33111122,\
        0xbb6969d2, 0x70d9d9a9, 0x898e8e07, 0xa7949433, 0xb69b9b2d,\
        0x221e1e3c, 0x92878715, 0x20e9e9c9, 0x49cece87, 0xff5555aa,\
        0x78282850, 0x7adfdfa5, 0x8f8c8c03, 0xf8a1a159, 0x80898909,\
        0x170d0d1a, 0xdabfbf65, 0x31e6e6d7, 0xc6424284, 0xb86868d0,\
        0xc3414182, 0xb0999929, 0x772d2d5a, 0x110f0f1e, 0xcbb0b07b,\
    0xfc5454a8, 0xd6bbbb6d, 0x3a16162c};
  
  uint32_t r0, r1, r2, r3; 
  uint32_t r = 1;
  
  
  m_c0 ^= m_workingKey[0][0];
  m_c1 ^= m_workingKey[0][1];
  m_c2 ^= m_workingKey[0][2];
  m_c3 ^= m_workingKey[0][3];
  
  
  while (r < m_rounds - 1)
  {
    r0 = (AES[m_c0 & 0xFF] & 0xFFFFFFFF);
    r0 ^= (shift(AES[(m_c1 >> 8) & 0xFF], 24) & 0xFFFFFFFF);
    r0 ^= (shift(AES[(m_c2 >> 16) & 0xFF], 16) & 0xFFFFFFFF);
    r0 ^= (shift(AES[(m_c3 >> 24) & 0xFF], 8) & 0xFFFFFFFF);
    r0 ^= (m_workingKey[r][0] & 0xFFFFFFFF);
    r1 = (AES[m_c1 & 0xFF] & 0xFFFFFFFF);
    r1 ^= shift(AES[(m_c2 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
    r1 ^= shift(AES[(m_c3 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
    r1 ^= shift(AES[(m_c0 >> 24) & 0xFF], 8) & 0xFFFFFFFF;
    r1 ^= m_workingKey[r][1] & 0xFFFFFFFF;
    r2 = AES[m_c2 & 0xFF] & 0xFFFFFFFF;
    r2 ^= shift(AES[(m_c3 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
    r2 ^= shift(AES[(m_c0 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
    r2 ^= shift(AES[(m_c1 >> 24) & 0xFF], 8) & 0xFFFFFFFF;
    r2 ^= m_workingKey[r][2] & 0xFFFFFFFF;
    r3 = AES[m_c3 & 0xFF] & 0xFFFFFFFF;
    r3 ^= shift(AES[(m_c0 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
    r3 ^= shift(AES[(m_c1 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
    r3 ^= shift(AES[(m_c2 >> 24) & 0xFF], 8) & 0xFFFFFFFF;
    r3 ^= m_workingKey[r][3] & 0xFFFFFFFF;
    r = r + 1;
    m_c0 = AES[r0 & 0xFF] & 0xFFFFFFFF;
    m_c0 ^= shift(AES[(r1 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
    m_c0 ^= shift(AES[(r2 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
    m_c0 ^= shift(AES[(r3 >> 24) & 0xFF], 8) & 0xFFFFFFFF;
    m_c0 ^= m_workingKey[r][0] & 0xFFFFFFFF;
    m_c1 = AES[r1 & 0xFF] & 0xFFFFFFFF;   
    m_c1 ^= shift(AES[(r2 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
    m_c1 ^= shift(AES[(r3 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
    m_c1 ^= shift(AES[(r0 >> 24) & 0xFF], 8) & 0xFFFFFFFF;
    m_c1 ^= m_workingKey[r][1] & 0xFFFFFFFF;
    m_c2 = AES[r2 & 0xFF] & 0xFFFFFFFF;
    m_c2 ^= shift(AES[(r3 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
    m_c2 ^= shift(AES[(r0 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
    m_c2 ^= shift(AES[(r1 >> 24) & 0xFF], 8) & 0xFFFFFFFF;
    m_c2 ^= m_workingKey[r][2] & 0xFFFFFFFF;
    m_c3 = AES[r3 & 0xFF] & 0xFFFFFFFF;
    m_c3 ^= shift(AES[(r0 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
    m_c3 ^= shift(AES[(r1 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
    m_c3 ^= shift(AES[(r2 >> 24) & 0xFF], 8) & 0xFFFFFFFF;
    m_c3 ^= m_workingKey[r][3] & 0xFFFFFFFF;
    r = r + 1;
  }
  
  r0 = AES[m_c0 & 0xFF] & 0xFFFFFFFF;
  r0 ^= shift(AES[(m_c1 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
  r0 ^= shift(AES[(m_c2 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
  r0 ^= shift(AES[m_c3 >> 24], 8) & 0xFFFFFFFF;
  r0 ^= m_workingKey[r][0] & 0xFFFFFFFF;
  r1 = AES[m_c1 & 0xFF] & 0xFFFFFFFF;
  r1 ^= shift(AES[(m_c2 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
  r1 ^= shift(AES[(m_c3 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
  r1 ^= shift(AES[m_c0 >> 24], 8) & 0xFFFFFFFF;
  r1 ^= m_workingKey[r][1] & 0xFFFFFFFF;
  r2 = AES[m_c2 & 0xFF] & 0xFFFFFFFF;
  r2 ^= shift(AES[(m_c3 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
  r2 ^= shift(AES[(m_c0 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
  r2 ^= shift(AES[m_c1 >> 24], 8) & 0xFFFFFFFF;
  r2 ^= m_workingKey[r][2] & 0xFFFFFFFF;
  r3 = AES[m_c3 & 0xFF] & 0xFFFFFFFF;
  r3 ^= shift(AES[(m_c0 >> 8) & 0xFF], 24) & 0xFFFFFFFF;
  r3 ^= shift(AES[(m_c1 >> 16) & 0xFF], 16) & 0xFFFFFFFF;
  r3 ^= shift(AES[m_c2 >> 24], 8) & 0xFFFFFFFF;
  r3 ^= m_workingKey[r][3] & 0xFFFFFFFF;
  r += 1;
  m_c0 = (S_BOX[r0 & 0xFF] & 0xFF) & 0xFFFFFFFF;
  m_c0 ^= ((S_BOX[(r1 >> 8) & 0xFF] & 0xFF) << 8) & 0xFFFFFFFF;
  m_c0 ^= ((S_BOX[(r2 >> 16) & 0xFF] & 0xFF) << 16) & 0xFFFFFFFF;
  m_c0 ^= ((S_BOX[r3 >> 24] & 0xFF) << 24) & 0xFFFFFFFF;
  m_c0 ^= m_workingKey[r][0] & 0xFFFFFFFF;
  m_c1 = (S_BOX[r1 & 0xFF] & 0xFF) & 0xFFFFFFFF;
  m_c1 ^= ((S_BOX[(r2 >> 8) & 0xFF] & 0xFF) << 8) & 0xFFFFFFFF;
  m_c1 ^= ((S_BOX[(r3 >> 16) & 0xFF] & 0xFF) << 16) & 0xFFFFFFFF;
  m_c1 ^= ((S_BOX[r0 >> 24] & 0xFF) << 24) & 0xFFFFFFFF;
  m_c1 ^= m_workingKey[r][1] & 0xFFFFFFFF;
  m_c2 = (S_BOX[r2 & 0xFF] & 0xFF) & 0xFFFFFFFF;
  m_c2 ^= ((S_BOX[(r3 >> 8) & 0xFF] & 0xFF) << 8) & 0xFFFFFFFF;
  m_c2 ^= ((S_BOX[(r0 >> 16) & 0xFF] & 0xFF) << 16) & 0xFFFFFFFF;
  m_c2 ^= ((S_BOX[r1 >> 24] & 0xFF) << 24) & 0xFFFFFFFF;
  m_c2 ^= m_workingKey[r][2] & 0xFFFFFFFF;
  m_c3 = (S_BOX[r3 & 0xFF] & 0xFF) & 0xFFFFFFFF;
  m_c3 ^= ((S_BOX[(r0 >> 8) & 0xFF] & 0xFF) << 8) & 0xFFFFFFFF;
  m_c3 ^= ((S_BOX[(r1 >> 16) & 0xFF] & 0xFF) << 16) & 0xFFFFFFFF;
  m_c3 ^= ((S_BOX[r2 >> 24] & 0xFF) << 24) & 0xFFFFFFFF;
  m_c3 ^= m_workingKey[r][3] & 0xFFFFFFFF;
}   
    
int Device_IM350::Data::getPosition()
{
  return m_position;
}

void Device_IM350::Data::set(byte *value, int count)
{
  m_dataSize = count;

  if(m_pData != NULL)
  {
    free(m_pData);
    m_pData = NULL;
  }
  
  m_pData = (byte *)malloc(m_dataSize);
  
  if(m_pOutput != NULL)
  {
    free(m_pOutput);
    m_pOutput = NULL;
  }
  
  m_pOutput = (byte *)malloc(m_dataSize);
  
  memcpy(m_pData, value, count);
}

void Device_IM350::Data::get(byte *target, int count)
{
  memcpy(target, m_pData+m_position, count);
  m_position += count;
}

byte Device_IM350::Data::getUInt8(int index = -1)
{
  byte value = 0;
  
  if (index == -1)
  {
    index = m_position;

    value = m_pData[index] & 0xFF;
    value = value % (byte)pow(2,8); 
    m_position += 1;
    return value;
  }

  value = m_pData[index] & 0xFF;  
  value = value % (byte)pow(2,8); // !?
  
  return value;
}

uint16_t Device_IM350::Data::getUInt16(int index = -1)
{
  uint16_t value = 0;
  
  if (index == -1)
  {
    index = m_position;
    value = ((m_pData[index] & 0xFF) << 8) | (m_pData[index+1] & 0xFF);   
    value % (uint16_t)pow(2, 16);
    m_position += 2;
    return value;
  }
  
  value = ((m_pData[index] & 0xFF) << 8) | (m_pData[index+1] & 0xFF);
  value % (uint16_t)pow(2, 16);
  return value; 
}

uint32_t Device_IM350::Data::getUInt32(int index = -1)
{
  uint32_t value = 0;

  if (index == -1)
  {
    index = m_position;
    m_position += 4;
  }
  
  value = m_pData[index] & 0xFF;
  value = value << 24;
  value |= (m_pData[index+1] & 0xFF) << 16;
  value |= (m_pData[index+2] & 0xFF) << 8;
  value |= (m_pData[index+3] & 0xFF);
  value % (uint16_t)pow(2, 32);
  return value;
}

int Device_IM350::Data::getObjectCount()
{
  int cnt = (int)getUInt8();
  if (cnt > 0x80)
  {
    if (cnt == 0x81)
      cnt = getUInt8();
    else if (cnt == 0x82)
      cnt = getUInt16();
    else if (cnt == 0x84)
      cnt = int(getUInt32());
  }
  
  return cnt;
}
