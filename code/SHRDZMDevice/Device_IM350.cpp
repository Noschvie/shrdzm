#include "Device_IM350.h"

#define UART_RXD_INV (BIT(19)) 

const byte firstByte = 0x7E; 
const byte firstByteSagemcom = 0xDB; 
const byte firstByteNO = 0x68; 
const byte lastByte = 0x7E; 
const int lenNormal = 123;
const int lenSagemcom = 511;
const int lenNO = 271;
const int lenDebug = 512;


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
  m_pConfigurationObject = NULL;
  baud = 115200;
  
  memset(meterTime, 0, 20);
  lastReadMessageLen = 0;
  cachedDataAvailable = false;
  wrongDebugSetupDetected = false;
  requestNeeded = true;

  lastReadMessage = new byte[lenDebug];

  done = false;
}

Device_IM350::~Device_IM350()
{  
  DLN(F("IM350 Instance deleted"));
}

bool Device_IM350::isNewDataAvailable()
{
  if(requestNeeded)     
  {
    return false;
  }

  cachedDataAvailable = readDataStream(false) > 0 || cachedDataAvailable ? true : false;

  return cachedDataAvailable;
}

bool Device_IM350::setDeviceParameter(JsonObject obj)
{ 
  inverted = false;
  
  if(obj.containsKey(F("requestpin")))
  {
    if( obj[F("requestpin")] == "-1" ||
        obj[F("requestpin")] == "OFF" ||
        obj[F("requestpin")] == "off")     
    {
      requestNeeded = false;
    }
    else
    {        
      requestNeeded = true;
      pinMode(atoi(obj[F("requestpin")]), OUTPUT);
      digitalWrite(atoi(obj[F("requestpin")]), LOW);   
    }
  }
  if(obj.containsKey(F("baud")))
  {
    baud = obj[F("baud")].as<long>();
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
      mySoftwareSerial.enableIntTx(false);
      mySoftwareSerial.begin(baud, SWSERIAL_8N1, obj[F("rxpin")].as<uint8_t>(), -1, inverted, 256);
      mySoftwareSerial.enableIntTx(false);
      softwareSerialUsed = true;    
    }
    else // hardwre serial is used
    {
      if(DEBUGGING_ENABLED == true &&  baud != SERIAL_BAUD)
      {
        wrongDebugSetupDetected = true;
      }    
      else
      {
        Serial.begin(baud);  

        Serial.println("Test");
        if(inverted)
                U0C0 = BIT(UCRXI) | BIT(UCBN) | BIT(UCBN+1) | BIT(UCSBN);        
        Serial.println("Test1");
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
  deviceParameter[F("baud")] = F("115200");
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

bool Device_IM350::readDataStream(bool withTimeout)
{
  uint32_t start_time = millis();
  byte tempChar = 0;
  int readCnt = 0;
  bool finished = false;
  uint32_t timeout=1000;

  while (!finished)
  {
    if (softwareSerialUsed ? mySoftwareSerial.available() : Serial.available()) 
    {
      tempChar = softwareSerialUsed ? mySoftwareSerial.read() : Serial.read(); 
      finished = true;
    }
    else
      finished = withTimeout ? !((millis() - start_time) < timeout) : true;
  } 

  if(tempChar == 0)
    return 0;

//  Serial.printf("%02X", tempChar);

  start_time = millis();
  timeout=1000;
  bool done = false;

  {
    memset(lastReadMessage, 0, lenDebug);
    lastReadMessage[0] = tempChar;
    lastReadMessageLen = 0;
    bool finished1 = false;
    bool finished2 = false;
    bool finished3 = false;

    readCnt = 1;

    if(tempChar == firstByte) // first test for vienna only!
    {    
      while(!finished1)
      {
        if (softwareSerialUsed ? mySoftwareSerial.available() : Serial.available()) 
        {
          finished1 = true;
          lastReadMessage[readCnt] = softwareSerialUsed ? mySoftwareSerial.read() : Serial.read();
//    Serial.printf("Address = %02X\n", lastReadMessage[readCnt]);
          readCnt++; 
          while(!finished2)
          {
            if (softwareSerialUsed ? mySoftwareSerial.available() : Serial.available()) 
            {
              finished2 = true;
              lastReadMessage[readCnt] = softwareSerialUsed ? mySoftwareSerial.read() : Serial.read();
//      Serial.printf("Len = HEX %02X\n", lastReadMessage[readCnt]);
//      Serial.printf("Len = DEC %02d\n", lastReadMessage[readCnt]);
            readCnt++; 
    
              while (!finished3) 
              {
                if (softwareSerialUsed ? mySoftwareSerial.available() : Serial.available()) 
                {
                  if(readCnt == lastReadMessage[2]+1)
                    finished3 = true;
                    
                  lastReadMessage[readCnt] = softwareSerialUsed ? mySoftwareSerial.read() : Serial.read(); 
//      Serial.printf("%02X", lastReadMessage[readCnt]);
                  readCnt++; 
                }            
                if((millis() - start_time) > 1700) // first timeout reached
                {
                  finished3 = true;
                }
              }
            }
            else
            {
              if((millis() - start_time) > 1500) // first timeout reached
              {
                finished2 = true;
              }
            }
          }
        }
        else
        {
          if((millis() - start_time) > 1000) // first timeout reached
          {
            finished1 = true;
//    Serial.println("finished1 Timeout reached");
          }
        }
      }
    }
    else
    {
      while ( readCnt < lenDebug && (millis() - start_time < timeout+1000) ) 
      {
        if (softwareSerialUsed ? mySoftwareSerial.available() : Serial.available()) 
        {
          lastReadMessage[readCnt] = softwareSerialUsed ? mySoftwareSerial.read() : Serial.read(); 
  
  //  Serial.printf("%02X", lastReadMessage[readCnt]);
          
          readCnt++; 
        }
      }
    }            

    lastReadMessageLen = readCnt;
  }

//  Serial.println();
  
  return readCnt > 0 ? true : false;
}

SensorData* Device_IM350::readParameter()
{  
  SensorData *al = NULL;  
  char str[15] = "";
  devicetype dt = unknown;

  if(wrongDebugSetupDetected)
  {
    al = new SensorData(2);
    
    al->di[0].nameI = F("lasterror");
    al->di[0].valueI = F("firmware is compiled for debug mode. This does not fit with the current serial settings.");  
    
    timeToString(str, sizeof(str));
    al->di[1].nameI = F("uptime");
    al->di[1].valueI = String(str);       

    return al;
  }

  String ck = deviceParameter[F("cipherkey")];

  if(!deviceParameter[F("autoRebootMinutes")].isNull())
  {
    if(strcmp(deviceParameter[F("autoRebootMinutes")],"0") != 0)
    {
      // Check whether to reboot first
      if(millis() > (atol(deviceParameter[F("autoRebootMinutes")]) * 1000 * 60))
      {
        DLN(F("Will reboot now"));
        
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

  bool newData = false;

  // if requestpin == -1, active request is disabled and smartmeter pushes the data without requesting
  if( !requestNeeded )     
  {
    newData = cachedDataAvailable;
    cachedDataAvailable = false;
  }
  else
  {
    digitalWrite(atoi(deviceParameter[F("requestpin")]), LOW);
  
    pinMode(LED_BUILTIN, OUTPUT); // LED als Output definieren
  
    // enable request
    digitalWrite(atoi(deviceParameter[F("requestpin")]), HIGH); 
    digitalWrite(LED_BUILTIN, LOW);

    newData = readDataStream(true);

    digitalWrite(atoi(deviceParameter[F("requestpin")]), LOW); 
    digitalWrite(LED_BUILTIN, HIGH); 
  }
    
  if(newData)
  {  
//    Serial.printf("%d bytes to interprete.\n", lastReadMessageLen);

    /// TEST generic 68er start
/*    if(lastReadMessage[0] == 0x68)
    {
      dt = start68;
    } */
    if(lastReadMessage[0] == firstByteSagemcom)  
    {
      dt = sagemcom;
    }
    else if(lastReadMessage[0] == firstByteNO && lastReadMessageLen == 376) // TINETZ, Salzburg, Vorarlberg
    {
      dt = kaifaMBus1;
    }
    else if(lastReadMessage[0] == firstByteNO)
    {
      dt = no;
    }
    else if(lastReadMessageLen == 110)
    {
      dt = e450;
    }
    else if(lastReadMessageLen == 105)
    {
      dt = e450IRWien;
    }
    else if(lastReadMessageLen == 123)
    {
      dt = im350;
    }
    else if(lastReadMessageLen == 121)
    {
      dt = am550;
    }
    else if(lastReadMessageLen == 114)
    {
      dt = im350Wels;
    }
    else if(lastReadMessageLen == 332)
    {
      dt = e450Steiermark;
    }
  }

  if(!newData)
  {
    if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
    {
      al = new SensorData(2);    
      
      al->di[0].nameI = F("lasterror");    
      al->di[0].valueI = F("No data read");  
              
      timeToString(str, sizeof(str));
      al->di[1].nameI = F("uptime");
      al->di[1].valueI = String(str);       

      return al;
    }
    else
    {        
      return NULL;
    }
  }
  else if (dt == unknown)
  {
    if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
    {    
      al = new SensorData(3);
      al->di[2].nameI = F("data");
      al->di[2].valueI = hexToString(lastReadMessage, lastReadMessageLen);

      al->di[0].nameI = F("lasterror");    
      al->di[0].valueI = F("No supported SmartMeter Type identified - No end Byte found");  
  
      timeToString(str, sizeof(str));
      al->di[1].nameI = F("uptime");
      al->di[1].valueI = String(str);       

      return al;
    }
    else
    {
      return NULL;
    }
  }  
  else
  {
    if(!init_vector(&Vector_SM, m_blockCipherKey, lastReadMessage, dt))
    {
      DLN("Consistency check failed");

      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(1);
        al->di[0].nameI = F("consistencyError");
        al->di[0].valueI = hexToString(lastReadMessage, lastReadMessageLen); 

        return al;
      }
      else
      {        
        return NULL;
      }      
    }
    
    byte bufferResult[Vector_SM.datasize+Vector_SM.datasize2+1];
    memset(bufferResult, 0, Vector_SM.datasize+Vector_SM.datasize2+1);

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

        if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
          al = new SensorData(obj.size()+2);
        else
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

        if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
        {
          al->di[counter].nameI = F("data");
          al->di[counter].valueI = hexToString(lastReadMessage, lastReadMessageLen);
          counter++;
        }
          
        timeToString(str, sizeof(str));
        al->di[counter].nameI = F("uptime");
        al->di[counter].valueI = String(str);       
      }
      else
      {
        if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
        {
          al = new SensorData(3);
          al->di[0].nameI = F("lasterror");    
          al->di[0].valueI = F("cipherkey does not fit");        
  
          timeToString(str, sizeof(str));
          al->di[1].nameI = F("uptime");
          al->di[1].valueI = String(str);       

          al->di[2].nameI = F("data");
          al->di[2].valueI = hexToString(lastReadMessage, lastReadMessageLen);

          return al;
        }
        else
          return NULL;
      }
    }
    else if((dt == am550 && 
              bufferResult[56] == 0x06 &&
              bufferResult[61] == 0x06 &&
              bufferResult[66] == 0x06 &&
              bufferResult[71] == 0x06) || dt == im350 || dt == im350Wels) // AM550/IM350 Carinthia, IM350 Wels
    {
      parse_message(bufferResult, dt);
   
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
        al = new SensorData(9);
      else
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
         
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al->di[8].nameI = F("data");
        al->di[8].valueI = hexToString(lastReadMessage, lastReadMessageLen);
      }      
    }
    else if(dt == am550 && bufferResult[38] == 0x06) // parse Slovenia am550
    {  
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(11);
        al->di[10].nameI = F("data");
        al->di[10].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
      }
      else
        al = new SensorData(10);

      sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (bufferResult[6] << 8) + (bufferResult[7]),bufferResult[8], bufferResult[9], bufferResult[11], bufferResult[12], bufferResult[13]);

      al->di[0].nameI = F("timestamp");
      al->di[0].valueI = String(meterTime); 

      al->di[1].nameI = F("value1");
      al->di[1].valueI = String(byteToUInt32(bufferResult, 39));         
      al->di[2].nameI = F("value2");
      al->di[2].valueI = String(byteToUInt32(bufferResult, 44));         
      al->di[3].nameI = F("value3");
      al->di[3].valueI = String(byteToUInt32(bufferResult, 49));         
      al->di[4].nameI = F("value4");
      al->di[4].valueI = String(byteToUInt32(bufferResult, 54));         
      al->di[5].nameI = F("value5");
      al->di[5].valueI = String(byteToUInt32(bufferResult, 59));         
      al->di[6].nameI = F("value6");
      al->di[6].valueI = String(byteToUInt32(bufferResult, 64));         
      al->di[7].nameI = F("value7");
      al->di[7].valueI = String(byteToUInt32(bufferResult, 69));         
      al->di[8].nameI = F("value8");
      al->di[8].valueI = String(byteToUInt32(bufferResult, 74));         

        
      timeToString(str, sizeof(str));
      al->di[9].nameI = F("uptime");
      al->di[9].valueI = String(str);          
    }
    else if(dt == e450IRWien)
    {
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(11);
        al->di[10].nameI = F("data");
        al->di[10].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
      }
      else
        al = new SensorData(10);

      sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (bufferResult[6] << 8) + (bufferResult[7]),bufferResult[8], bufferResult[9], bufferResult[11], bufferResult[12], bufferResult[13]);

      al->di[0].nameI = F("timestamp");
      al->di[0].valueI = String(meterTime);  

      al->di[1].nameI = "1.8.0";
      al->di[1].valueI = String(byteToUInt32(bufferResult,35));         

      al->di[2].nameI = "2.8.0";
      al->di[2].valueI = String(byteToUInt32(bufferResult,40));         

      al->di[3].nameI = "3.8.0";
      al->di[3].valueI = String(byteToUInt32(bufferResult,45));         

      al->di[4].nameI = "4.8.0";
      al->di[4].valueI = String(byteToUInt32(bufferResult,50));         

      al->di[5].nameI = "1.7.0";
      al->di[5].valueI = String(byteToUInt32(bufferResult,55));         

      al->di[6].nameI = "2.7.0";
      al->di[6].valueI = String(byteToUInt32(bufferResult,60));         

      al->di[7].nameI = "3.7.0";
      al->di[7].valueI = String(byteToUInt32(bufferResult,65));         

      al->di[8].nameI = "4.7.0";
      al->di[8].valueI = String(byteToUInt32(bufferResult,70));         

      timeToString(str, sizeof(str));
      al->di[9].nameI = F("uptime");
      al->di[9].valueI = String(str);                           
    }
    else if(dt == e450Steiermark)
    {
      char bh[6];
      bh[5] = 0;
      
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(8);
        al->di[7].nameI = F("data");
        al->di[7].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
      }
      else
        al = new SensorData(7);
      
      sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (bufferResult[6] << 8) + (bufferResult[7]),bufferResult[8], bufferResult[9], bufferResult[11], bufferResult[12], bufferResult[13]);

      al->di[0].nameI = F("timestamp");
      al->di[0].valueI = String(meterTime);  

      sprintf(bh, "%d.%d.%d", bufferResult[38], bufferResult[39],bufferResult[40]);
      al->di[1].nameI = bh;
      al->di[1].valueI = String(byteToUInt32(bufferResult,43));         

      sprintf(bh, "%d.%d.%d", bufferResult[51], bufferResult[52],bufferResult[53]);
      al->di[2].nameI = bh;
      al->di[2].valueI = String(byteToUInt32(bufferResult,56));         
                          
      sprintf(bh, "%d.%d.%d", bufferResult[64], bufferResult[65],bufferResult[66]);
      al->di[3].nameI = bh;
      al->di[3].valueI = String(byteToUInt32(bufferResult,69));         

      sprintf(bh, "%d.%d.%d", bufferResult[77], bufferResult[78],bufferResult[79]);
      al->di[4].nameI = bh;
      al->di[4].valueI = String(byteToUInt32(bufferResult,82));         

      sprintf(bh, "%d.%d.%d", bufferResult[90], bufferResult[91],bufferResult[92]);
      al->di[5].nameI = bh;
      al->di[5].valueI = String(byteToUInt32(bufferResult,95));                

      timeToString(str, sizeof(str));
      al->di[6].nameI = F("uptime");
      al->di[6].valueI = String(str);                     
    }
    else if(dt == e450) // parse e450
    {
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(13);
        al->di[12].nameI = F("data");
        al->di[12].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
      }
      else
        al = new SensorData(12);
      
      sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (bufferResult[6] << 8) + (bufferResult[7]),bufferResult[8], bufferResult[9], bufferResult[11], bufferResult[12], bufferResult[13]);

      al->di[0].nameI = F("timestamp");
      al->di[0].valueI = String(meterTime);     

      al->di[1].nameI = F("32.7.0");
      al->di[1].valueI = String(byteToUInt16(bufferResult,21));         
  
      al->di[2].nameI = F("52.7.0");
      al->di[2].valueI = String(byteToUInt16(bufferResult,24));         

      al->di[3].nameI = F("72.7.0");
      al->di[3].valueI = String(byteToUInt16(bufferResult,27));         

      al->di[4].nameI = F("31.7.0");
      al->di[4].valueI = String(byteToUInt16(bufferResult,30));         

      al->di[5].nameI = F("51.7.0");
      al->di[5].valueI = String(byteToUInt16(bufferResult,33));         

      al->di[6].nameI = F("71.7.0");
      al->di[6].valueI = String(byteToUInt16(bufferResult,36));         

      al->di[7].nameI = F("1.7.0");
      al->di[7].valueI = String(byteToUInt32(bufferResult,39));         

      al->di[8].nameI = F("2.7.0");
      al->di[8].valueI = String(byteToUInt32(bufferResult,44));         

      al->di[9].nameI = F("1.8.0");
      al->di[9].valueI = String(byteToUInt32(bufferResult,49));         

      al->di[10].nameI = F("2.8.0");
      al->di[10].valueI = String(byteToUInt32(bufferResult,54));         

      timeToString(str, sizeof(str));
      al->di[11].nameI = F("uptime");
      al->di[11].valueI = String(str);                     
    }
    else if(dt == no) // parse niederoesterreich
    {
      char bh[6];
      bh[5] = 0;

      // simple consistency check
      if( bufferResult[38] != 1 || bufferResult[39] != 8 || bufferResult[40] != 0 ||
          bufferResult[57] != 2 || bufferResult[58] != 8 || bufferResult[59] != 0 ||
          bufferResult[76] != 1 || bufferResult[77] != 7 || bufferResult[78] != 0 ||
          bufferResult[95] != 2 || bufferResult[96] != 7 || bufferResult[97] != 0 ||
          bufferResult[114] != 32 || bufferResult[115] != 7 || bufferResult[116] != 0 ||
          bufferResult[131] != 52 || bufferResult[132] != 7 || bufferResult[133] != 0 ||
          bufferResult[148] != 72 || bufferResult[149] != 7 || bufferResult[150] != 0 ||
          bufferResult[165] != 31 || bufferResult[166] != 7 || bufferResult[167] != 0 ||
          bufferResult[182] != 51 || bufferResult[183] != 7 || bufferResult[184] != 0 ||
          bufferResult[199] != 71 || bufferResult[200] != 7 || bufferResult[201] != 0 ||
          bufferResult[216] != 13 || bufferResult[217] != 7 || bufferResult[218] != 0)
      {
        DLN("Consistency check failed");

        if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
        {
          al = new SensorData(1);
          al->di[0].nameI = F("consistencyError");
          al->di[0].valueI = hexToString(lastReadMessage, lastReadMessageLen); 

          return al;
        }
        else
        {        
          return NULL;
        }
      }
      
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(14);
        al->di[13].nameI = F("data");
        al->di[13].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
      }
      else
        al = new SensorData(13);
      
      sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (bufferResult[6] << 8) + (bufferResult[7]),bufferResult[8], bufferResult[9], bufferResult[11], bufferResult[12], bufferResult[13]);

      al->di[0].nameI = F("timestamp");
      al->di[0].valueI = String(meterTime);  

      sprintf(bh, "%d.%d.%d", bufferResult[38], bufferResult[39],bufferResult[40]);
      al->di[1].nameI = bh;
      al->di[1].valueI = String(byteToUInt32(bufferResult,43));         

      sprintf(bh, "%d.%d.%d", bufferResult[57], bufferResult[58],bufferResult[59]);
      al->di[2].nameI = bh;
      al->di[2].valueI = String(byteToUInt32(bufferResult,62));         
                          
      sprintf(bh, "%d.%d.%d", bufferResult[76], bufferResult[77],bufferResult[78]);
      al->di[3].nameI = bh;
      al->di[3].valueI = String(byteToUInt32(bufferResult,81));         

      sprintf(bh, "%d.%d.%d", bufferResult[95], bufferResult[96],bufferResult[97]);
      al->di[4].nameI = bh;
      al->di[4].valueI = String(byteToUInt32(bufferResult,100));         

      sprintf(bh, "%d.%d.%d", bufferResult[114], bufferResult[115],bufferResult[116]);
      al->di[5].nameI = bh;
      al->di[5].valueI = String(byteToUInt16(bufferResult,119));         

      sprintf(bh, "%d.%d.%d", bufferResult[131], bufferResult[132],bufferResult[133]);
      al->di[6].nameI = bh;
      al->di[6].valueI = String(byteToUInt16(bufferResult,136));         

      sprintf(bh, "%d.%d.%d", bufferResult[148], bufferResult[149],bufferResult[150]);
      al->di[7].nameI = bh;
      al->di[7].valueI = String(byteToUInt16(bufferResult,153));         

      sprintf(bh, "%d.%d.%d", bufferResult[165], bufferResult[166],bufferResult[167]);
      al->di[8].nameI = bh;
      al->di[8].valueI = String(byteToUInt16(bufferResult,170));         

      sprintf(bh, "%d.%d.%d", bufferResult[182], bufferResult[183],bufferResult[184]);
      al->di[9].nameI = bh;
      al->di[9].valueI = String(byteToUInt16(bufferResult,187));         

      sprintf(bh, "%d.%d.%d", bufferResult[199], bufferResult[200],bufferResult[201]);
      al->di[10].nameI = bh;
      al->di[10].valueI = String(byteToUInt16(bufferResult,204));         

      sprintf(bh, "%d.%d.%d", bufferResult[216], bufferResult[217],bufferResult[218]);
      al->di[11].nameI = bh;
      al->di[11].valueI = String(byteToUInt16(bufferResult,221));         

      timeToString(str, sizeof(str));
      al->di[12].nameI = F("uptime");
      al->di[12].valueI = String(str);                     
    }
    else if(dt == kaifaMBus1)
    {
      char bh[6];
      bh[5] = 0;

      // simple consistency check
      if( bufferResult[102] != 32 || bufferResult[103] != 7 || bufferResult[104] != 0 ||
          bufferResult[121] != 52 || bufferResult[122] != 7 || bufferResult[123] != 0)
      {
        DLN("Consistency check failed");

        if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
        {
          al = new SensorData(1);
          al->di[0].nameI = F("consistencyError");
          al->di[0].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
        }
        else
        {        
          return NULL;
        }
      }
      
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(15);
        al->di[14].nameI = F("data");
        al->di[14].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
      }
      else
        al = new SensorData(14);
      
      sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (bufferResult[6] << 8) + (bufferResult[7]),bufferResult[8], bufferResult[9], bufferResult[11], bufferResult[12], bufferResult[13]);

      al->di[0].nameI = F("timestamp");
      al->di[0].valueI = String(meterTime);  

      sprintf(bh, "%d.%d.%d", bufferResult[102], bufferResult[103],bufferResult[104]);
      al->di[1].nameI = bh;
      al->di[1].valueI = String(byteToUInt16(bufferResult,107));         

      sprintf(bh, "%d.%d.%d", bufferResult[121], bufferResult[122],bufferResult[123]);
      al->di[2].nameI = bh;
      al->di[2].valueI = String(byteToUInt16(bufferResult,126));         

      sprintf(bh, "%d.%d.%d", bufferResult[140], bufferResult[141],bufferResult[142]);
      al->di[3].nameI = bh;
      al->di[3].valueI = String(byteToUInt16(bufferResult,145));         

      sprintf(bh, "%d.%d.%d", bufferResult[159], bufferResult[160],bufferResult[161]);
      al->di[4].nameI = bh;
      al->di[4].valueI = String(byteToUInt16(bufferResult,164));         

      sprintf(bh, "%d.%d.%d", bufferResult[178], bufferResult[179],bufferResult[180]);
      al->di[5].nameI = bh;
      al->di[5].valueI = String(byteToUInt16(bufferResult,183));         

      sprintf(bh, "%d.%d.%d", bufferResult[197], bufferResult[198],bufferResult[199]);
      al->di[6].nameI = bh;
      al->di[6].valueI = String(byteToUInt16(bufferResult,202));         

      sprintf(bh, "%d.%d.%d", bufferResult[216], bufferResult[217],bufferResult[218]);
      al->di[7].nameI = bh;
      al->di[7].valueI = String(byteToUInt32(bufferResult,221));         
      
      sprintf(bh, "%d.%d.%d", bufferResult[237], bufferResult[238],bufferResult[239]);
      al->di[8].nameI = bh;
      al->di[8].valueI = String(byteToUInt32(bufferResult,242));         
      
      sprintf(bh, "%d.%d.%d", bufferResult[258], bufferResult[259],bufferResult[260]);
      al->di[9].nameI = bh;
      al->di[9].valueI = String(byteToUInt32(bufferResult,263));         
      
      sprintf(bh, "%d.%d.%d", bufferResult[279], bufferResult[280],bufferResult[281]);
      al->di[10].nameI = bh;
      al->di[10].valueI = String(byteToUInt32(bufferResult,284));
      
      sprintf(bh, "%d.%d.%d", bufferResult[300], bufferResult[301],bufferResult[302]);
      al->di[11].nameI = bh;
      al->di[11].valueI = String(byteToUInt32(bufferResult,305));
      
      sprintf(bh, "%d.%d.%d", bufferResult[321], bufferResult[322],bufferResult[323]);
      al->di[12].nameI = bh;
      al->di[12].valueI = String(byteToUInt32(bufferResult,326));      

      timeToString(str, sizeof(str));
      al->di[13].nameI = F("uptime");
      al->di[13].valueI = String(str);                     
    }
    else
    {
      if(!deviceParameter[F("sendRawData")].isNull() && strcmp(deviceParameter[F("sendRawData")],"YES") == 0)
      {
        al = new SensorData(3);
        al->di[2].nameI = F("data");
        al->di[2].valueI = hexToString(lastReadMessage, lastReadMessageLen); 
      }
      else
        al = new SensorData(2);
      
      al->di[0].nameI = F("lasterror");    
      al->di[0].valueI = F("Invalid data");  

      timeToString(str, sizeof(str));
      al->di[1].nameI = F("uptime");
      al->di[1].valueI = String(str);          
    }            
        
    return al;
  }
}

byte Device_IM350::hexToByte(const char* code)
{
   return strtoul(code, NULL, 16);
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
  DLN(F("file opened..."));
  
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
  DLN(F("file opened..."));
  
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

String Device_IM350::hexToString(byte array[], int readCnt)
{
  String data;
  char hexCode[3];
  hexCode[2] = 0;
    
  for(int i = 0; i<readCnt; i++)
  {
    sprintf(hexCode, "%02X", array[i]);
    data += String(hexCode);    
  }  

  return data;
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

uint16_t Device_IM350::byteToUInt16(byte array[], unsigned int startByte)
{
    uint16_t result;
    result = (uint16_t) array[startByte] << 8;
    result |=  (uint16_t) array[startByte+1];

    return result;
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

void Device_IM350::parse_message(byte array[], devicetype dt) 
{  
  int startPos = 0;

  if(dt == im350 || dt == am550)
    startPos = 57;  
  else if(dt == im350Wels)
    startPos = 52;
  
  sprintf(meterTime, "%02d-%02d-%02dT%02d:%02d:%02d", (array[6] << 8) + (array[7]),array[8], array[9], array[11], array[12], array[13]);
  DV(meterTime);
    
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

  if(vect->datasize2 == 0)
  {
    gcmaes128->decrypt((byte*)bufferResult, (byte*)vect->ciphertextPos, vect->datasize);
    DLN(hexToString((byte*)vect->ciphertextPos, vect->datasize));
  }
  else
  {
    byte combinedCiphertext[vect->datasize+vect->datasize2];

    memcpy(combinedCiphertext, (byte*)vect->ciphertextPos, vect->datasize);
    memcpy(combinedCiphertext+vect->datasize, (byte*)vect->ciphertextPos2, vect->datasize2);
    
    gcmaes128->decrypt((byte*)bufferResult, combinedCiphertext, vect->datasize+vect->datasize2);
    DLN(hexToString((byte*)combinedCiphertext, vect->datasize+vect->datasize2));    
  }
  
  delete gcmaes128;

  DLN(hexToString(bufferResult, vect->datasize+vect->datasize2));
}

bool Device_IM350::init_vector(Vector_GCM *vect, byte *key_SM, byte *readMessage, devicetype dt) 
{
  int inaddi = 0;
  int inaddi2 = 0;
  int iv1start = 0;
  int iv2start = 0;  
  byte tag[] = {0,0,0,0,0,0,0,0,0,0,0,0};   
  
  vect->ciphertextPos2 = NULL;
  vect->datasize2 = 0;
  
  for (int i = 0; i < 12; i++) 
  {
    vect->tag[i] = tag[i];
  }  

  if(dt == start68)
  {
    // check if L Field 2 times the same
    if (readMessage[1] != readMessage[2] || readMessage[3] != 0x68)
      return false;

    // if message len at least the len of a frame
    if(lastReadMessageLen < readMessage[1] + 6)
      return false;
    
//    Serial.printf("EndFrame Char = %02X\n", readMessage[readMessage[1]+5]);

    // if stop character at correct position
    if(readMessage[readMessage[1]+5] != 0x16)
      return false;

    Serial.printf("Checksum Char = %02X\n", readMessage[readMessage[1]+4]);
    byte checksumCalc = readMessage[4];

    // claculate checksum
    for(int i = 5; i < readMessage[1]+3; i++)
    {
      checksumCalc += readMessage[i];
    }

    Serial.printf("Checksum Calculated1 = %02X\n", checksumCalc);
    checksumCalc += 1;
    Serial.printf("Checksum Calculated2 = %02X\n", checksumCalc);
    
    
    // check CI fiels - how many frames
    

  }
  
  if(dt == im350)
  {
    inaddi = 30;
    vect->datasize = 90;
    iv1start = 16;
    iv2start = 18;  
  }
  else if(dt == no)
  {
    inaddi = 26;
    vect->datasize = 243;
    iv1start = 11;
    iv2start = 14;  
  }
  else if(dt == kaifaMBus1)
  {
    inaddi = 27;
    inaddi2 = 265;
    vect->datasize = 227;
    vect->datasize2 = 109;
    iv1start = 11;
    iv2start = 15;     
    vect->ciphertextPos2 = readMessage+inaddi2;
  }
  else if(dt == am550)
  {
    inaddi = 28;
    vect->datasize = 90;
    iv1start = 14;
    iv2start = 16;  
  }
  else if(dt == e450)
  {
    inaddi = 27;
    vect->datasize = 80;
    iv1start = 13;
    iv2start = 15;  
  }
  else if(dt == e450IRWien)
  {
    inaddi = 28;
    vect->datasize = 74;
    iv1start = 14;
    iv2start = 16;  
  }
  else if(dt == e450Steiermark)
  {
    inaddi = 37;
    vect->datasize = 293;
    iv1start = 21;
    iv2start = 25;  
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

  DLN(hexToString(vect->iv, 12));
    
  vect->authsize = 16;
  vect->tagsize = 12;
  vect->ivsize  = 12;  

  return true;
}
