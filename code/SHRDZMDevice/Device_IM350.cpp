#include "Device_IM350.h"

#define UART_RXD_INV (BIT(19)) 
#define DEBUG_SHRDZM

const byte firstByte = 0x7E; 
const byte firstByteSagemcom = 0xDB; 
const byte lastByte = 0x7E; 
const int lenNormal = 123;
const int lenSagemcom = 511;

/*Vector_GCM Vector_SM;
GCM<AES128> *gcmaes128 = 0;
*/

  // IM350
//  String c2 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92916FC1EF62AB5F476F8A59772745CC99365500ACF5EBEEA82F95581762C2D18804A1E7F7700FB10752F04D9344779C6A332C973EAF3CF375095D1821E87C68909EE47759AD925976C42E4D92FF9727E4213FDEE5F1ABE45D9D97F5E5E7E";
//  String c2 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92A57400A90D827A575C749DAB31F44F3C4B83D9B15F9FD20D9FE147F5AD3A0432A65208408AE177E31AF4CD985DAD92F7647BB5ED8F619F629059606BBC55128D5E9A23BB992F564D098BBE8E759ECC0A01B259A02599A236EC45475A07E";
//  String c1 = "7EA079CF0002002313D986E6E700DB08534D53677004C4465F200001A92A57400A90D827A575C749DAB31F44F3C4B83D9B15F9FD20D9FE147F5AD3A0432A65208408AE177E31AF4CD985DAD92F7647BB5ED8F619F629059606BBC55128D5E9A23BB992F564D098BBE8E759ECC0A01B259A02599A236EC45475A07E";  
//  String ckSimulation = "612888F59306A7855AC5C08CB3A96B39"; // IM350
//    String adSimulation = "D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"; // IM350

  // AM550
//  String c1 = "7EA077CF022313BB45E6E700DB0849534B69745CF6C95F200000046BB45F8F43980519E16E36BC0C7198EB0E9CF1E0DF6A227D63C3F24852486F4A0CE8AA187991BF1C44544F62BBBB75BEEB4CDB549D7CF525CD95E6D82D2BE1C7F2ED84505402317F84AEB2A285508EC1B072553C39CD928F1F492941F67E";  
//  String c2 = "7EA077CF022313BB45E6E700DB0849534B69745CF6C95F200000046BB45F8F43980519E16E36BC0C7198EB0E9CF1E0DF6A227D63C3F24852486F4A0CE8AA187991BF1C44544F62BBBB75BEEB4CDB549D7CF525CD95E6D82D2BE1C7F2ED84505402317F84AEB2A285508EC1B072553C39CD928F1F492941F67E";  
//  String ckSimulation = "62F9445B069BA866068B8036F2612030"; // AM550

  // Helmut (Wels)
//  String c1 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000BE34111B482F835CF05AACA84AE6BEDD7EE0F17132CEC104495BD7E9C7F258775072AB713296B1E4CC9BE98468E24D194237365949209A5D6CA42584EB9BB7F5F7E50E5304886F3DA7A9342D157553B2E48A175D907E";  
//  String c2 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000BE34111B482F835CF05AACA84AE6BEDD7EE0F17132CEC104495BD7E9C7F258775072AB713296B1E4CC9BE98468E24D194237365949209A5D6CA42584EB9BB7F5F7E50E5304886F3DA7A9342D157553B2E48A175D907E";  
//  String c2 = "7EA070CF0002002313E0C7E6E700DB08534D53677002FD6256200000C06514BA5DA79232B4036C892317BA7AAF692564DC5DDE0AC93A0997E52F44A5A1FC0505E81AE07CEBFB2845FB73D2D2B8608CE8DC9E6761AC970D3658CE754AA82421E1F05A7BE06D5CDE45995D3E7A596D896D317E";    
//  String ckSimulation = "5A9AFEB2A17E6CF0CBAF98C8300ED9D7"; // Wels

  // Sagemcom
//  String c1 = "DB08534147350000D28F8201F230000050A5F569179254242B91A93C6A120EFD5FC8344D78351D1F0EF1A43F3C7545496374A7EC7DC175B5E836C79AA5AB7A40B359CF8EA911BA8C1C6B22CBB0F9AF44689F96221C183FCE646E770B54F32A7F48DC0AA2C10AAFD9BDA84EFA07731496EE94A0A288545BE53D7686270AE14CE38AF997B8497CAC4D3FF824BFFE51229BD7AC4C1A24E5BD7FD994E8BA4730662E4AD3C6646D9D21B66C8221D0A9D677552161F0EBC1D1399CE8366F060CC61225820BC5D2C1F3DEC94EDD36A021120D5AA372E308A2AFB119E55C1846597BC031F55005F08360DF578E1BE40F1237A81CCA187BF536278BAA422021716C39BEF9C2F364FDE526F4E6EAE354879A5CFFD433B1887C5DEB5FEAA3281CCD526E96FCF587D57CE11480B399981E8D1560532D31F944F8896D7CE7938E7DCC76F93074DE90CCF3A65198894029B959FC685983BA5739FBC5D8378ACA3C1C8F48487A4F295B9F29198F50E6342AF502ED5E268CECF7D0141A8348C9F1BC0EB8E8A9E47CBF96BFF2C74DEEF5DD977F5A30E4086DB60C3D6963443254306E96292568089225D4AB1E2DA6425EAF30CA07B915E5FBC90732B18E635EED493ABC6A2C7B6D9EADCEF97EF261B336630DABE74BE89A814ED9662CFDCAF95B610637CFE4DF927150DC7E6DDE3198FF9B56399188B51C44A302DB7D39C74764474FABB075724D";  
//  String c2 = "DB08534147350000D28F8201F230000050A5F569179254242B91A93C6A120EFD5FC8344D78351D1F0EF1A43F3C7545496374A7EC7DC175B5E836C79AA5AB7A40B359CF8EA911BA8C1C6B22CBB0F9AF44689F96221C183FCE646E770B54F32A7F48DC0AA2C10AAFD9BDA84EFA07731496EE94A0A288545BE53D7686270AE14CE38AF997B8497CAC4D3FF824BFFE51229BD7AC4C1A24E5BD7FD994E8BA4730662E4AD3C6646D9D21B66C8221D0A9D677552161F0EBC1D1399CE8366F060CC61225820BC5D2C1F3DEC94EDD36A021120D5AA372E308A2AFB119E55C1846597BC031F55005F08360DF578E1BE40F1237A81CCA187BF536278BAA422021716C39BEF9C2F364FDE526F4E6EAE354879A5CFFD433B1887C5DEB5FEAA3281CCD526E96FCF587D57CE11480B399981E8D1560532D31F944F8896D7CE7938E7DCC76F93074DE90CCF3A65198894029B959FC685983BA5739FBC5D8378ACA3C1C8F48487A4F295B9F29198F50E6342AF502ED5E268CECF7D0141A8348C9F1BC0EB8E8A9E47CBF96BFF2C74DEEF5DD977F5A30E4086DB60C3D6963443254306E96292568089225D4AB1E2DA6425EAF30CA07B915E5FBC90732B18E635EED493ABC6A2C7B6D9EADCEF97EF261B336630DABE74BE89A814ED9662CFDCAF95B610637CFE4DF927150DC7E6DDE3198FF9B56399188B51C44A302DB7D39C74764474FABB075724D";  
//  String c2 = "DB08534147350000D28F8201F230000050A5F569179254242B91A93C6A120EFD5FC8344D78351D1F0EF1A43F3C7545496374A7EC7DC175B5E836C79AA5AB7A40B359CF8EA911BA8C1C6B22CBB0F9AF44689F96221C183FCE646E770B54F32A7F48DC0AA2C10AAFD9BDA84EFA07731496EE94A0A288545BE53D7686270AE14CE38AF997B8497CAC4D3FF824BFFE51229BD7AC4C1A24E5BD7FD994E8BA4730662E4AD3C6646D9D21B66C8221D0A9D677552161F0EBC1D1399CE8366F060CC61225820BC5D2C1F3DEC94EDD36A021120D5AA372E308A2AFB119E55C1846597BC031F55005F08360DF578E1BE40F1237A81CCA187BF536278BAA422021716C39BEF9C2F364FDE526F4E6EAE354879A5CFFD433B1887C5DEB5FEAA3281CCD526E96FCF587D57CE11480B399981E8D1560532D31F944F8896D7CE7938E7DCC76F93074DE90CCF3A65198894029B959FC685983BA5739FBC5D8378ACA3C1C8F48487A4F295B9F29198F50E6342AF502ED5E268CECF7D0141A8348C9F1BC0EB8E8A9E47CBF96BFF2C74DEEF5DD977F5A30E4086DB60C3D6963443254306E96292568089225D4AB1E2DA6425EAF30CA07B915E5FBC90732B18E635EED493ABC6A2C7B6D9EADCEF97EF261B336630DABE74BE89A814ED9662CFDCAF95B610637CFE4DF927150DC7E6DDE3198FF9B56399188B51C44A302DB7D39C74764474FABB075724D";    
//  String ckSimulation = "953E7F1725EA910F26E053813A639094"; // Steiermark

Device_IM350::Device_IM350()
{    
  softwareSerialUsed = false;
  inverted = false;
  dt = unknown;
  m_pConfigurationObject = NULL;

  memset(meterTime, 0, 20);
  
  done = false;
}

Device_IM350::~Device_IM350()
{
  Serial.println(F("IM350 Instance deleted"));
}

bool Device_IM350::isNewDataAvailable()
{
  return true;
}

bool Device_IM350::setDeviceParameter(JsonObject obj)
{ 
  inverted = false;
  
  if(obj.containsKey(F("requestpin")))
  {
    pinMode(atoi(obj[F("requestpin")]), OUTPUT);
    digitalWrite(atoi(obj[F("requestpin")]), LOW);   
  }
  if(obj.containsKey(F("cipherkey")))
  {
    String codeBuffer(obj[F("cipherkey")].as<char*>());
    codeBuffer.replace( " ", "" );
    
    if(codeBuffer.length() == 32)
    {
      memcpy(m_cipherkey, codeBuffer.c_str(), 32);
      hexToBytes(m_cipherkey, m_blockCipherKey);

      obj[F("cipherkey")] = codeBuffer;
    }
    else
      return false;
  }  
  if(obj.containsKey(F("invertrx")))
  {
    String codeBuffer(obj[F("invertrx")].as<char*>());
    
    if(codeBuffer[0] == 'Y' || 
       codeBuffer[0] == 'y' ||
       codeBuffer[0] == '1' ||
       codeBuffer[0] == 'T' ||
       codeBuffer[0] == 't'
       )
    {
      obj[F("invertrx")] = F("YES");
      inverted = true;
    }
    else
    {
      obj[F("invertrx")] = F("NO");
    }    
  }  

  if(obj.containsKey(F("rxpin")))
  {
    if(obj[F("rxpin")].as<uint8_t>() != 3) // if not pin 3, software serial is needed
    {
      Serial.println(F("Device is running with SoftwareSerial"));

      mySoftwareSerial.begin(115200, SWSERIAL_8N1, obj[F("rxpin")].as<uint8_t>(), -1, inverted, 256);
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

  if(m_pConfigurationObject != NULL)
    interval = atoi((*m_pConfigurationObject)["interval"]);

  return true;
}

void Device_IM350::prepare()
{
}

bool Device_IM350::initialize()
{
  deviceParameter = doc.to<JsonObject>();
  deviceParameter[F("requestpin")] = F("5");
  deviceParameter[F("cipherkey")] = F("00000000000000000000000000000000");
  deviceParameter[F("rxpin")] = F("3");
  deviceParameter[F("invertrx")] = F("NO");
  deviceParameter[F("sendRawData")] = F("NO");
  deviceParameter[F("autoRebootMinutes")] = F("0");

  return true;
}

SensorData* Device_IM350::readParameterTypes()
{
  SensorData *al = new SensorData(6);

  al->di[0].nameI = F("counter_reading_p_in");
  al->di[1].nameI = F("counter_reading_p_out");
  al->di[2].nameI = F("counter_reading_q_in");
  al->di[3].nameI = F("counter_reading_q_out");
  al->di[4].nameI = F("current_power_usage_in");
  al->di[5].nameI = F("current_power_usage_out");

  return al;
}

SensorData* Device_IM350::readInitialSetupParameter()
{
  SensorData *al = new SensorData(1);

  al->di[0].nameI = F("interval");
  al->di[0].valueI = F("120");

  return al; 
}

SensorData* Device_IM350::readParameter()
{  
  SensorData *al = NULL;  
  char str[15] = "";
  char hexCode[3];
  hexCode[2] = 0;
  
  String ck = deviceParameter[F("cipherkey")];

  if(!deviceParameter[F("autoRebootMinutes")].isNull())
  {
    if(strcmp(deviceParameter[F("autoRebootMinutes")],"0") != 0)
    {
      // Check whether to reboot first
      if(millis() > (atol(deviceParameter[F("autoRebootMinutes")]) * 1000 * 60))
      {
        Serial.println(F("Will reboot now"));
        
        delay(500); 
        ESP.restart();      
      }
    }
  }

  if(ck == F("00000000000000000000000000000000") || ck.length() != 32)
  {
    al = new SensorData(2);
    
    al->di[0].nameI = F("lasterror");
    al->di[0].valueI = F("cipherkey not set!");  
    
    timeToString(str, sizeof(str));
    al->di[1].nameI = F("uptime");
    al->di[1].valueI = String(str);       

    return al;
  }

  digitalWrite(atoi(deviceParameter[F("requestpin")]), LOW);

  // clear serial buffer
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
  digitalWrite(atoi(deviceParameter[F("requestpin")]), HIGH); 
  digitalWrite(LED_BUILTIN, LOW);
  
  uint32_t timeout=2000;

  bool finished = false;
  int cnt = 0;
  uint32_t start_time = millis();
  byte tempChar = 0;
  byte *readMessage;
  uint32_t messageLen = 0;

  if(softwareSerialUsed)
  {
    while ((tempChar != firstByte && tempChar != firstByteSagemcom) && (millis() - start_time < timeout)) 
    {
      if (mySoftwareSerial.available()) 
      {
          tempChar = mySoftwareSerial.read(); 
      }
    }
    
    if (tempChar == firstByte) // IM350/AM550
    {
      readMessage = new byte[123];
      memset(readMessage, 0, 123);
      readMessage[0] = tempChar;
      messageLen = 123;
      
      cnt++;
      while ( cnt < 123 && (millis() - start_time < timeout) ) 
      {
        if (mySoftwareSerial.available()) 
        {
          readMessage[cnt] = mySoftwareSerial.read(); 
          cnt++; 
        }
      }
    }       
    else if(tempChar == firstByteSagemcom) // TEST!!!!
    {
      cnt++;
      
      readMessage = new byte[lenSagemcom];
      memset(readMessage, 0, lenSagemcom);
      readMessage[0] = tempChar;
      messageLen = lenSagemcom;

      while ( cnt < lenSagemcom && (millis() - start_time < timeout+2000) ) 
      {
        if (mySoftwareSerial.available()) 
        {
          readMessage[cnt] = mySoftwareSerial.read(); 
          cnt++; 
        }
      }
    }
  }
  else
  {
    while ((tempChar != 0x7E  && tempChar != firstByteSagemcom) && (millis() - start_time < timeout)) 
    {
      if (Serial.available()) 
      {
        tempChar = Serial.read(); 
      }
    }
    if (tempChar == firstByte) 
    {
      readMessage = new byte[123];
      memset(readMessage, 0, 123);
      readMessage[0] = tempChar;
      messageLen = 123;  

      cnt++;      
      while ( cnt < 123 && (millis() - start_time < timeout) ) 
      {
        if (Serial.available()) 
        {
          readMessage[cnt] = Serial.read(); 
          cnt++; 
        }
      }
    }  
    else if(tempChar == firstByteSagemcom) // TEST!!!!
    {
      cnt++;

      readMessage = new byte[lenSagemcom];
      memset(readMessage, 0, lenSagemcom);
      readMessage[0] = tempChar;
      messageLen = lenSagemcom;
      
      while ( cnt < lenSagemcom && (millis() - start_time < timeout+2000) ) 
      {
        if (Serial.available()) 
        {
          readMessage[cnt] = Serial.read(); 
          cnt++; 
        }
      }
    }    
  }

  // disable request
  digitalWrite(atoi(deviceParameter[F("requestpin")]), LOW); 
  digitalWrite(LED_BUILTIN, HIGH);


  if(!softwareSerialUsed)
  {
    Serial.flush();
    Serial.end();
    Serial.begin(SERIAL_BAUD);
  }

  if(messageLen > 10)
  {  
    if(readMessage[0] == firstByteSagemcom)  
    {
      dt = sagemcom;
      Serial.println("sagemcom detected");
    }
    else if(readMessage[messageLen-1] == lastByte)
    {
      dt = im350;
      Serial.println("im350 detected");
    }
    else if (readMessage[messageLen-3] == lastByte)
    {
      dt = am550;
      Serial.println("am550 detected");
    }
    else if (readMessage[messageLen-10] == lastByte)
    {
      dt = im350Wels;
      Serial.println("im350Wels detected");
    }
  }

  if(tempChar == 0)
  {
    al = new SensorData(2);    
    
    al->di[0].nameI = F("lasterror");    
    al->di[0].valueI = F("No data read");  
            
    timeToString(str, sizeof(str));
    al->di[1].nameI = F("uptime");
    al->di[1].valueI = String(str);       
    
    return al;    
  }
  else if (dt == unknown)
  {
    if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
    {    
      al = new SensorData(3);
      al->di[2].nameI = F("data");
      al->di[2].valueI = String((char*)readMessage);            
    }
    else
    {
      al = new SensorData(2);
    }
    
    al->di[0].nameI = F("lasterror");    
    al->di[0].valueI = F("No supported SmartMeter Type identified - No end Byte found");  

    timeToString(str, sizeof(str));
    al->di[1].nameI = F("uptime");
    al->di[1].valueI = String(str);       
            
    return al;
  }  
  else // IM350 or AM550 or T210 read
  {
    init_vector(&Vector_SM, m_blockCipherKey, readMessage, dt); 
    byte bufferResult[Vector_SM.datasize+1];
    memset(bufferResult, 0, Vector_SM.datasize+1);

    decrypt_text(&Vector_SM, bufferResult);

    if(dt == sagemcom)
    {
      char *posBuffer = NULL;
      int posBufferPosition = 0;

      if(bufferResult[0] != NULL && bufferResult[0] == 47) // first char must be a slash
      {
        posBuffer = strstr( (char *)bufferResult, "0-0:1.0.0" );  
      }
      
      if(posBuffer != NULL)
      {
        String data((char *)bufferResult);
        uint8_t counter = 0;
        
        posBufferPosition = posBuffer - (char *)bufferResult;
  
        data = "{"+data.substring(posBufferPosition);
        int endChar = data.lastIndexOf('!');

        if(endChar > 0)
        {
          data = data.substring(0, endChar-1);
          data.replace(",\r\n", "}");
        }
  
        data.replace("0-0:", "\"");
        data.replace("1-0:", "\"");
        data.replace("(", "\" : \"");
        data.replace(")", "\",");
        data.replace("*Wh", "");
        data.replace("*W", "");
        data.replace("*varh", "");
        data.replace("*var", "");

        
        DynamicJsonDocument doc(800);
        deserializeJson(doc, data);
        JsonObject obj = doc.as<JsonObject>();
  
        al = new SensorData(obj.size()+1);
  
        for (JsonPair kv : obj) 
        {      
          al->di[counter].nameI = String(kv.key().c_str());
  
          if(strcmp(kv.key().c_str(), "1.0.0") == 0)
            al->di[counter].valueI = String(kv.value().as<char*>());
          else
            al->di[counter].valueI = String(atol(kv.value().as<char*>()), DEC);
          
          counter++;
        }
          
        timeToString(str, sizeof(str));
        al->di[counter].nameI = F("uptime");
        al->di[counter].valueI = String(str);       
      }
      else
      {
        al = new SensorData(2);
        
        al->di[0].nameI = F("lasterror");    
        al->di[0].valueI = F("cipherkey does not fit");        

        timeToString(str, sizeof(str));
        al->di[1].nameI = F("uptime");
        al->di[1].valueI = String(str);       
      }
    }
    else
    {
      Serial.println("vor parse_message");    
      parse_message(bufferResult);

      al = new SensorData(8);

      al->di[0].nameI = F("counter_reading_p_in");
      al->di[0].valueI = String(counter_reading_p_in);  
    
      al->di[1].nameI = F("counter_reading_p_out");
      al->di[1].valueI = String(counter_reading_p_out);  
    
      al->di[2].nameI = F("counter_reading_q_in");
      al->di[2].valueI = String(counter_reading_q_in);  
    
      al->di[3].nameI = F("counter_reading_q_out");
      al->di[3].valueI = String(counter_reading_q_out);  
    
      al->di[4].nameI = F("counter_power_usage_in");
      al->di[4].valueI = String(current_power_usage_in);  
    
      al->di[5].nameI = F("counter_power_usage_out");
      al->di[5].valueI = String(current_power_usage_out); 
    
      al->di[6].nameI = F("timestamp");
      al->di[6].valueI = String(meterTime); 
    
      timeToString(str, sizeof(str));
      al->di[7].nameI = F("uptime");
      al->di[7].valueI = String(str);       
    }
        
    delete readMessage;

    return al;
  }
}


/////////////////////////////
uint32_t Device_IM350::writeHourValue(const char *value)
{
  char content[20];
  memset(content, 0, 20);
#ifdef LITTLEFS  
  File hourFile = LittleFS.open("/IM350_hour.txt", "r");
#else
  File hourFile = SPIFFS.open("/IM350_hour.txt", "r");
#endif
  Serial.println("file opened...");
  
  if (hourFile) 
  {
    for(int i=0;i<hourFile.size();i++) //Read upto complete file size
    {
      content[i] += (char)hourFile.read();
      if(i == 19)
      {
        hourFile.close();      
        return 0;
      }
    }  
  }
  
  hourFile.close();  

#ifdef LITTLEFS  
  LittleFS.remove("/IM350_hour.txt");
  hourFile = LittleFS.open("/IM350_hour.txt", "w");
#else
  SPIFFS.remove("/IM350_hour.txt");
  hourFile = SPIFFS.open("/IM350_hour.txt", "w");  
#endif
  
  if (hourFile) 
  {
    hourFile.write(value, strlen(value));
  }

  hourFile.close();  

  return atol(content);
}

uint32_t Device_IM350::writeDayValue(const char *value)
{
  char content[20];
  memset(content, 0, 20);
#ifdef LITTLEFS  
  File dayFile = LittleFS.open("/IM350_day.txt", "r");
#else
  File dayFile = SPIFFS.open("/IM350_day.txt", "r");
#endif
  Serial.println("file opened...");
  
  if (dayFile) 
  {
    for(int i=0;i<dayFile.size();i++) //Read upto complete file size
    {
      content[i] += (char)dayFile.read();
      if(i == 19)
      {
        dayFile.close();      
        return 0;
      }
    }  
  }
  
  dayFile.close();  

#ifdef LITTLEFS  
  LittleFS.remove("/IM350_day.txt");
  dayFile = LittleFS.open("/IM350_day.txt", "w");
#else
  SPIFFS.remove("/IM350_day.txt");
  dayFile = SPIFFS.open("/IM350_day.txt", "w");  
#endif
  
  if (dayFile) 
  {
    dayFile.write(value, strlen(value));
  }

  dayFile.close();  

  return atol(content);
}

time_t Device_IM350::tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
/*  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet); */

  return NULL;
}

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
  int startPos = 0;

  if(dt == im350 || dt == am550)
    startPos = 57;  
  else if(dt == im350Wels)
    startPos = 52;
  
  sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (array[6] << 8) + (array[7]),array[8], array[9], array[11], array[12], array[13]);
  
  counter_reading_p_in = byteToUInt32(array, startPos);
  counter_reading_p_out = byteToUInt32(array, startPos+5);
  counter_reading_q_in = byteToUInt32(array, startPos+10);
  counter_reading_q_out = byteToUInt32(array, startPos+15);
  current_power_usage_in = byteToUInt32(array, startPos+20);
  current_power_usage_out = byteToUInt32(array, startPos+25);
}

void Device_IM350::decrypt_text(Vector_GCM *vect, byte *bufferResult)
{
  gcmaes128 = new GCM<AES128>();
  gcmaes128->setKey(vect->key, gcmaes128->keySize());
  gcmaes128->setIV(vect->iv, vect->ivsize);
  gcmaes128->decrypt((byte*)bufferResult, (byte*)vect->ciphertextPos, vect->datasize);
  delete gcmaes128;
}

void Device_IM350::init_vector(Vector_GCM *vect, byte *key_SM, byte *readMessage, devicetype dt) 
{
  int inaddi = 0;
  int iv1start = 0;
  int iv2start = 0;  
  byte tag[] = {0,0,0,0,0,0,0,0,0,0,0,0}; 

  for (int i = 0; i < 12; i++) 
  {
    vect->tag[i] = tag[i];
  }  
  
  if(dt == im350)
  {
    inaddi = 30;
    vect->datasize = 90;
    iv1start = 16;
    iv2start = 18;  
  }
  else if(dt == am550)
  {
    inaddi = 28;
    vect->datasize = 90;
    iv1start = 14;
    iv2start = 16;  
  }
  else if(dt == im350Wels)
  {
    inaddi = 30;
    vect->datasize = 81;
    iv1start = 16;
    iv2start = 18;  
  }
  else if(dt == sagemcom)
  {
    inaddi = 18;
    vect->datasize = 481;
    iv1start = 2;
    iv2start = 6;  
  }
  
  vect->name = "Smartmeter";  // vector name
  for (unsigned int i = 0; i < 16; i++) 
  {
    vect->key[i] = key_SM[i];
  }

  vect->ciphertextPos = readMessage+inaddi;

  for (int i = 0; i < 8; i++) 
  {
     vect->iv[i] = readMessage[iv1start+i]; // manufacturer + serialnumber 8 bytes
  }
  for (int i = 8; i < 12; i++) 
  {
     vect->iv[i] = readMessage[iv2start+i]; // frame counter
  }
    
  vect->authsize = 16;
  vect->tagsize = 12;
  vect->ivsize  = 12;  
}
