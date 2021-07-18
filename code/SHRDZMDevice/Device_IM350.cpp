#include "Device_IM350.h"

//#define SIMULATION
#define UART_RXD_INV (BIT(19)) 
//#define DEBUG_SHRDZM

const byte firstByte = 0x7E; 
const byte lastByte = 0x7E; 
const int waitTime = 1500;

Vector_GCM Vector_SM;
GCM<AES128> *gcmaes128 = 0;

#ifdef SIMULATION

  // IM350
//  String c2 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92916FC1EF62AB5F476F8A59772745CC99365500ACF5EBEEA82F95581762C2D18804A1E7F7700FB10752F04D9344779C6A332C973EAF3CF375095D1821E87C68909EE47759AD925976C42E4D92FF9727E4213FDEE5F1ABE45D9D97F5E5E7E";
//  String c2 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92A57400A90D827A575C749DAB31F44F3C4B83D9B15F9FD20D9FE147F5AD3A0432A65208408AE177E31AF4CD985DAD92F7647BB5ED8F619F629059606BBC55128D5E9A23BB992F564D098BBE8E759ECC0A01B259A02599A236EC45475A07E";
//  String c1 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92A57400A90D827A575C749DAB31F44F3C4B83D9B15F9FD20D9FE147F5AD3A0432A65208408AE177E31AF4CD985DAD92F7647BB5ED8F619F629059606BBC55128D5E9A23BB992F564D098BBE8E759ECC0A01B259A02599A236EC45475A07E";  
//  String ckSimulation = "612888F59306A7855AC5C08CB3A96B39"; // IM350

  // AM550
  String c1 = "7EA077CF022313BB45E6E700DB0849534B69745CF6C95F200000046BB45F8F43980519E16E36BC0C7198EB0E9CF1E0DF6A227D63C3F24852486F4A0CE8AA187991BF1C44544F62BBBB75BEEB4CDB549D7CF525CD95E6D82D2BE1C7F2ED84505402317F84AEB2A285508EC1B072553C39CD928F1F492941F67E";  
  String c2 = "7EA077CF022313BB45E6E700DB0849534B69745CF6C95F200000046BB45F8F43980519E16E36BC0C7198EB0E9CF1E0DF6A227D63C3F24852486F4A0CE8AA187991BF1C44544F62BBBB75BEEB4CDB549D7CF525CD95E6D82D2BE1C7F2ED84505402317F84AEB2A285508EC1B072553C39CD928F1F492941F67E";  
  String ckSimulation = "62F9445B069BA866068B8036F2612030"; // AM550

  // Helmut (Wels)
//  String c1 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000BE34111B482F835CF05AACA84AE6BEDD7EE0F17132CEC104495BD7E9C7F258775072AB713296B1E4CC9BE98468E24D194237365949209A5D6CA42584EB9BB7F5F7E50E5304886F3DA7A9342D157553B2E48A175D907E";  
//  String c2 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000BE34111B482F835CF05AACA84AE6BEDD7EE0F17132CEC104495BD7E9C7F258775072AB713296B1E4CC9BE98468E24D194237365949209A5D6CA42584EB9BB7F5F7E50E5304886F3DA7A9342D157553B2E48A175D907E";  
//  String c2 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000C06514BA5DA79232B4036C892317BA7AAF692564DC5DDE0AC93A0997E52F44A5A1FC0505E81AE07CEBFB2845FB73D2D2B8608CE8DC9E6761AC970D3658CE754AA82421E1F05A7BE06D5CDE45995D3E7A596D896D317E";    
//  String ckSimulation = "5A9AFEB2A17E6CF0CBAF98C8300ED9D7"; // Wels
  
  int number = 1;
#endif

Device_IM350::Device_IM350()
{    
  softwareSerialUsed = false;
  inverted = false;
  dt = unknown;
  
  done = false;
}

Device_IM350::~Device_IM350()
{
  Serial.println("IM350 Instance deleted");
}

bool Device_IM350::isNewDataAvailable()
{
  return true;
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
      Serial.println("Device is running with SoftwareSerial");

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
  deviceParameter = doc.to<JsonObject>();
  deviceParameter["requestpin"] = "5";
  deviceParameter["cipherkey"] = "00000000000000000000000000000000";
  deviceParameter["rxpin"] = "3";
  deviceParameter["invertrx"] = "NO";
  deviceParameter["sendRawData"] = "NO";
  deviceParameter["autoRebootMinutes"] = "0";

  return true;
}

SensorData* Device_IM350::readParameterTypes()
{
  SensorData *al = new SensorData(6);

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
  String data;
  char str[15] = "";

  memset(message, 0, 123);

#ifdef SIMULATION
  String ck = ckSimulation;
  memcpy(m_cipherkey, ck.c_str(), 32);
  
  if(number == 1)
  {
    data = c1;
    number = 2;
  }
  else
  {
    data = c2;
    number = 1;
  }  
#else
  String ck = deviceParameter["cipherkey"];

  if(!deviceParameter["autoRebootMinutes"].isNull())
  {
    if(strcmp(deviceParameter["autoRebootMinutes"],"0") != 0)
    {
      // Check whether to reboot first
      if(millis() > (atol(deviceParameter["autoRebootMinutes"]) * 1000 * 60))
      {
        Serial.println("Will reboot now");
        
        delay(500); 
        ESP.restart();      
      }
    }
  }

  if(ck == "00000000000000000000000000000000" || ck.length() != 32)
  {
    al = new SensorData(1);
    
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "cipherkey not set!";  
    
    return al;
  }

  digitalWrite(atoi(deviceParameter["requestpin"]), LOW);

  // clean serial buffer
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
  
  uint32_t start_time = millis();
  uint32_t timeout=2000;

  bool finished = false;
  memset(message, 0, 123);
  int cnt = 0;
  
  if(softwareSerialUsed)
  {
    while (message[0] != 0x7E && (millis() - start_time < timeout)) 
    {
      if (mySoftwareSerial.available()) 
      {
        message[0] = mySoftwareSerial.read(); 
      }
    }
    if (message[0] == firstByte) 
    {
      cnt++;
      while ( cnt < 123 && (millis() - start_time < timeout) ) 
      {
        if (mySoftwareSerial.available()) 
        {
          message[cnt] = mySoftwareSerial.read(); 
          cnt++; 
        }
      }
    }       
  }
  else
  {
    while (message[0] != 0x7E && (millis() - start_time < timeout)) 
    {
      if (Serial.available()) 
      {
        message[0] = Serial.read(); 
      }
    }
    if (message[0] == firstByte) 
    {
      cnt++;
      while ( cnt < 123 && (millis() - start_time < timeout) ) 
      {
        if (Serial.available()) 
        {
          message[cnt] = Serial.read(); 
          cnt++; 
        }
      }
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

  int count = 0;
  
  if(message[sizeof(message)-1] == lastByte)
    count = 123;
  else if (message[sizeof(message)-3] == lastByte)
    count = 121;
  else if (message[sizeof(message)-10] == lastByte)
    count = 114;

  char hexCode[3];
  hexCode[2] = 0;
  
  for(int i = 0; i<count; i++)
  {
    sprintf(hexCode, "%02X", message[i]);
    data += String(hexCode);    
  }

#endif

  // analyse smartmeter type
  if(data.length() == 246)
    dt = im350;
  else if(data.length() == 242)
    dt = am550;
  else if(data.length() == 228)
    dt = im350Wels;

  if (dt == unknown)
  {
    if(!deviceParameter["sendRawData"].isNull() && strcmp(deviceParameter["sendRawData"],"YES") == 0)
    {    
      al = new SensorData(2);
      al->di[1].nameI = "data";
      al->di[1].valueI = data;
    }
    else
    {
      al = new SensorData(1);
    }
    
    al->di[0].nameI = "lasterror";
    al->di[0].valueI = "No supported SmartMeter Type identified - No end Byte found";  
            
    return al;
  }  

  hexToBytes(data.c_str(), message);
  hexToBytes(m_cipherkey, m_blockCipherKey);
  init_vector(&Vector_SM,"Vector_SM",m_blockCipherKey, dt); 
  decrypt_text(&Vector_SM);
  parse_message(buffer);

  if(!deviceParameter["sendRawData"].isNull() && strcmp(deviceParameter["sendRawData"],"YES") == 0)
  {
    al = new SensorData(8);
    al->di[7].nameI = "encoded";
    al->di[7].valueI = String(data.c_str());          
  }
  else
  {
    al = new SensorData(7);
  }

  al->di[0].nameI = "counter_reading_p_in";
  al->di[0].valueI = String(counter_reading_p_in);  

  al->di[1].nameI = "counter_reading_p_out";
  al->di[1].valueI = String(counter_reading_p_out);  

  al->di[2].nameI = "counter_reading_q_in";
  al->di[2].valueI = String(counter_reading_q_in);  

  al->di[3].nameI = "counter_reading_q_out";
  al->di[3].valueI = String(counter_reading_q_out);  

  al->di[4].nameI = "counter_power_usage_in";
  al->di[4].valueI = String(current_power_usage_in);  

  al->di[5].nameI = "counter_power_usage_out";
  al->di[5].valueI = String(current_power_usage_out); 

  timeToString(str, sizeof(str));
  al->di[6].nameI = "uptime";
  al->di[6].valueI = String(str); 
  
  return al;  
}


/////////////////////////////
void Device_IM350::timeToString(char* string, size_t size)
{
  unsigned long nowMillis = millis();
  unsigned long seconds = nowMillis / 1000;
  int days = seconds / 86400;
  seconds %= 86400;
  byte hours = seconds / 3600;
  seconds %= 3600;
  byte minutes = seconds / 60;
  seconds %= 60;
  snprintf(string, size, "%04d:%02d:%02d:%02d", days, hours, minutes, seconds);
}

void Device_IM350::hexToBytes(const char* code, byte* result)
{
  if(code != NULL)
  {
    String bb(code);
    char hexCode[2];
    
    bb.replace( " ", "" );
    
    for(int i = 0; i<bb.length() / 2; i++)
    {
      result[i] = strtoul(bb.substring(i*2, i*2+2).c_str(), NULL, 16);
    }   
  } 
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

void Device_IM350::parse_message(byte array[]) 
{  
      counter_reading_p_in = byteToUInt32(array, 57);
      counter_reading_p_out = byteToUInt32(array, 62);
      counter_reading_q_in = byteToUInt32(array, 67);
      counter_reading_q_out = byteToUInt32(array, 72);
      current_power_usage_in = byteToUInt32(array, 77);
      current_power_usage_out = byteToUInt32(array, 82);

      // Serial.println(result, DEC);
      Serial.printf("counter_reading_p_in: %d\n", counter_reading_p_in);
      Serial.printf("counter_reading_p_out: %d\n", counter_reading_p_out);
      Serial.printf("counter_reading_q_in: %d\n", counter_reading_q_in);
      Serial.printf("counter_reading_q_out: %d\n", counter_reading_q_out);
      Serial.printf("current_power_usage_in: %d\n", current_power_usage_in);
      Serial.printf("current_power_usage_out: %d\n", current_power_usage_out);
}

void Device_IM350::decrypt_text(Vector_GCM *vect) 
{
  gcmaes128 = new GCM<AES128>();
  gcmaes128->setKey(vect->key, gcmaes128->keySize());
  gcmaes128->setIV(vect->iv, vect->ivsize);
  gcmaes128->decrypt((byte*)buffer, vect->ciphertext, vect->datasize);
  delete gcmaes128;
}

// *****************************************************************************************
void Device_IM350::init_vector(Vector_GCM *vect, const char *Vect_name, byte *key_SM, devicetype dt) 
{
  int inaddi = 0;
  int len = 0;
  int iv1start = 0;
  int iv2start = 0;  
  
  if(dt == im350)
  {
    inaddi = 30;
    len = 90;
    iv1start = 16;
    iv2start = 18;  
  }
  else if(dt == am550)
  {
    inaddi = 28;
    len = 90;
    iv1start = 14;
    iv2start = 16;  
  }
  else if(dt == im350Wels)
  {
    inaddi = 30;
    len = 81;
    iv1start = 16;
    iv2start = 18;  
  }
  
  vect->name = Vect_name;  // vector name
  for (unsigned int i = 0; i < 16; i++) 
  {
    vect->key[i] = key_SM[i];
  }

  for (unsigned int i = 0; i < len; i++) 
  {
    vect->ciphertext[i] = message[i+inaddi];
  }

  byte AuthData[] = {0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf}; // fixed value, i got it from the gurus director software

  for (int i = 0; i < 16; i++) 
  {
     vect->authdata[i] = AuthData[i];
  }
  for (int i = 0; i < 8; i++) 
  {
     vect->iv[i] = message[iv1start+i]; // manufacturer + serialnumber 8 bytes
  }
  for (int i = 8; i < 12; i++) 
  {
    vect->iv[i] = message[iv2start+i]; // frame counter
  }
  
  byte tag[12]; // 12x zero
  for (int i = 0; i < 12; i++) {
    vect->tag[i] = tag[i];
  }
  vect->authsize = 16;
  vect->datasize = len;
  vect->tagsize = 12;
  vect->ivsize  = 12;
}
