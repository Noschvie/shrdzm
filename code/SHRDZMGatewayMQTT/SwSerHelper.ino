

void SwSerLoop()
{
//  while (swSer.available()) 
  if (swSer.available()) 
  {
    char r = swSer.read();
    
    if ( r == '*' ) 
    {
      String cmd = readSerialSS();

      sendSensorData(cmd);      
    }
    else if(r == '#' ) 
    {
      sendGatewayUpdate(readGatewayReply());
    }
    else if(r == '~' ) 
    {
      String cmd = readSerialSS();

      DLN("Gateway message : "+cmd);
      handleGatewayMessage(cmd);
    }
  }
}

String readSerialSS()
{
  String cmd = "";
  byte inByte = 0;
  int counter = 0;
  bool finished = false;
  unsigned int timeoutStart = millis();

  while (!finished) 
  {
    if(swSer.available())
    {
      char inChar = (char)swSer.read();
      if (inChar == '\n') 
      {
        finished = true;
      }
      else
      {
        cmd += inChar;
        timeoutStart = millis();
      }    
    }
    else if(timeoutStart + 2000 < millis())
    {
      finished = true;
    }    
  } 
  
  return cmd.substring(3);
}

String readGatewayReply()
{
  String cmd = "";
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
        cmd += (char)inByte;
      }
    }    
  }  

  delay(100);
  
  return cmd;  
}

void sendGatewayUpdate(String data)
{
  DLN("GatewayUpdate sent : "+data);


}

void handleGatewayMessage(String cmd)
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
  }
}

void sendSensorData(String data)
{
  StringSplitter *splitter = new StringSplitter(data, '$', 3);
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
        mqttclient.publish((String(MQTT_TOPIC)+"/"+splitter->getItemAtIndex(1)+"/sensor/"+t).c_str(), 
          v.c_str());

        if(strcmp(configuration.getCloudParameter("enabled"),"true") == 0 && cloudConnected)
        {
          cloudAddMeasurement(splitter->getItemAtIndex(1).c_str(), t.c_str(), v.c_str());
        }
      }
      else if(splitter->getItemAtIndex(0) == "[I]")  // Init
      {
        mqttclient.publish((String(MQTT_TOPIC)+"/state").c_str(), String(t+" "+v).c_str());
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
          registerDeviceTypeBuffer = splitter->getItemAtIndex(1)+":"+v;
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
