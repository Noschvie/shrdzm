char serBuffer[MAXLINELENGTH];
uint8_t sum1 = 0;
uint8_t sum2 = 0;

void SwSerLoop()
{
  if (swSer.available()) 
  {
    char r = swSer.read();
    
    if ( r == '*' ) 
    {
      if(readSerialSS())
      {
        yield(); 
        sendSensorData();      
      }
    }
    else if(r == '#' ) 
    {
      readGatewayReply();
      yield(); 
      sendGatewayUpdate();
    }
    else if(r == '~' ) 
    {
      if(readSerialSS())
      {
        yield(); 
        handleGatewayMessage();
      }
    }
  }
}

bool readSerialSS()
{
  byte inByte = 0;
  int counter = 0;
  uint16_t checksum = 0;
    
  swSer.readBytes(cmd, 3);
  if(cmd[0] == 'A')// checksum added
  {
    memcpy(&checksum, cmd+1, 2);
  }
  
  int len = swSer.readBytesUntil('\n', cmd, MAXLINELENGTH);
  cmd[len] = '\0';

  if(checksum != 0)
  {
    sum1 = 0;
    sum2 = 0;
    
    for(int i = 0; i<strlen(cmd); i++)
    {
      sum1 = (sum1 + cmd[i]) % 255;
      sum2 = (sum2 + sum1) % 255;
    }        

    if(((sum2 << 8) | sum1) != checksum)
    {
      sendChecksumError();
      return false;
    }
  }

  DV(cmd);

  return true;
}

void readGatewayReply()
{
  byte inByte = 0;
  int counter = 0;
  bool finished = false;
  
  while (!finished)
  {
    while (swSer.available() == 0 && counter < 50) 
    { 
      delay(1); 
      counter++;
    }
    if(counter == 50)
    {
      finished = true;
    }
    else
    {
      counter = 0;
      inByte = swSer.read();

      yield();
      
      if (inByte == '\n')
      {
        finished = true;
      }
      else
      {
        cmd[counter++] = (char)inByte;
      }
    }    
  }  

  cmd[counter] = '\0';

  delay(100);
}

void sendGatewayUpdate()
{
//  DLN("GatewayUpdate sent : "+*data);


}

void handleGatewayMessage()
{
  StringSplitter *splitter = new StringSplitter(cmd, '$', 4);
  int itemCount = splitter->getItemCount();

  if(splitter->getItemAtIndex(0) == "[G]")
  {
    StringSplitter *splitter1 = new StringSplitter(splitter->getItemAtIndex(1), ':', 2);
    int itemCount1 = splitter1->getItemCount();
  
    if(itemCount1 == 2)
    {
      if(splitter1->getItemAtIndex(0) == "address")
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/gatewayaddress").c_str(), splitter1->getItemAtIndex(1).c_str());
      }
    }

    delete splitter1;
  }
  else if(splitter->getItemAtIndex(0) == "[V]")  // gateway version
  {
    StringSplitter *splitter1 = new StringSplitter(splitter->getItemAtIndex(1), ':', 2);
    int itemCount1 = splitter1->getItemCount();
  
    if(itemCount1 == 2)
    {
      if(splitter1->getItemAtIndex(0) == "version")
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/gatewayversion").c_str(), splitter1->getItemAtIndex(1).c_str());
      }
    }

    delete splitter1;
  }

  delete splitter;
}

void sendChecksumError()
{
  mqttclient.publish((String(MQTT_TOPIC)+"/LastError").c_str(), "Checksum Error");
}

void sendSensorData()
{
  StringSplitter *splitter = new StringSplitter(cmd, '$', 3);
  int itemCount = splitter->getItemCount();
  String subject = "SHRDZM/";

  if(itemCount == 3)
  {
    if(splitter->getItemAtIndex(2).indexOf(':') > 0)
    {
      String v = splitter->getItemAtIndex(2).substring(splitter->getItemAtIndex(2).indexOf(':')+1);
      String t = splitter->getItemAtIndex(2).substring(0, splitter->getItemAtIndex(2).indexOf(':'));

      if(splitter->getItemAtIndex(0) == "[D]")
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/sensor/"+
        splitter->getItemAtIndex(2).substring(0, splitter->getItemAtIndex(2).indexOf(':'))).c_str(), 
          splitter->getItemAtIndex(2).substring(splitter->getItemAtIndex(2).indexOf(':')+1).c_str());

        if(strcmp(configuration.getCloudParameter("enabled"),"true") == 0 && cloudConnected)
        {
          cloudAddMeasurement(splitter->getItemAtIndex(1).c_str(), 
          splitter->getItemAtIndex(2).substring(0, splitter->getItemAtIndex(2).indexOf(':')).c_str(), 
          splitter->getItemAtIndex(2).substring(splitter->getItemAtIndex(2).indexOf(':')+1).c_str());
        }
      }
      else if(splitter->getItemAtIndex(0) == "[I]")  // Init
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/state").c_str(), 
        (splitter->getItemAtIndex(2).substring(0, splitter->getItemAtIndex(2).indexOf(':'))+" "+
        splitter->getItemAtIndex(2).substring(splitter->getItemAtIndex(2).indexOf(':')+1)).c_str());
      }  
      else if(splitter->getItemAtIndex(0) == "[P]") // [P]$500291D60619$paired:OK
      {        
        mqttclient.publish((String(MQTT_TOPIC)+"/paired").c_str(), 
          String(deviceName+"/"+splitter->getItemAtIndex(1)).c_str());
      }      
      else if(splitter->getItemAtIndex(0) == "[C]") // [C]$F4CFA2D4009B$interval:30
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/config").c_str(), 
          splitter->getItemAtIndex(2).c_str());

        if(t == "devicetype")
        {
          freeForRegistering = false;
          registerDeviceTypeBuffer = splitter->getItemAtIndex(1)+":"+v;
        }
      }
      else if(splitter->getItemAtIndex(0) == "[A]")
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/actions").c_str(), 
          splitter->getItemAtIndex(2).c_str());
      }
    }
    else
    {
      if(splitter->getItemAtIndex(0) == "[I]")
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/init").c_str(), 
          splitter->getItemAtIndex(2).c_str());
      }
      else if(splitter->getItemAtIndex(0) == "[V]")
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/version").c_str(), 
          splitter->getItemAtIndex(2).c_str());
      }
      else if(splitter->getItemAtIndex(0) == "[P]")
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/param").c_str(), 
          splitter->getItemAtIndex(2).c_str());
      }
      else if(splitter->getItemAtIndex(0) == "[X]")  // Sensors
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/sensors").c_str(), 
          splitter->getItemAtIndex(2).c_str());

        freeForRegistering = true;
      }  
      else
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/error").c_str(), 
          splitter->getItemAtIndex(2).c_str());
      }
    }
  }

  delete splitter;         
}
