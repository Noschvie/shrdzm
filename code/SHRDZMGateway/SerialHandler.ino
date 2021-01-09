char inChar;
String inputString;

void handleSerial()
{
  while (Serial.available()) 
  {
    inChar = (char)Serial.read();
  
    if (inChar == '\n') 
    {
        DV(inputString);
        
        if(inputString == "reset")  // reset me
        {
          ESP.restart();      
        }    
        else if(inputString.substring(0,7) == "$pair") // 
        {
          simpleEspConnection.startPairing(30);
        }                          
        else if(inputString.substring(0,9) == "$upgrade ") // Upgrade Gateway
        {
          StringSplitter *splitter = new StringSplitter(inputString, ' ', 4);
          int itemCount = splitter->getItemCount();
          
          if(itemCount == 2)
          {
            // SSID, password, host
            DLN("starting updateFirmware... ");
            updateFirmware(splitter->getItemAtIndex(1));
          }        
        }
        
        inputString = "";        
    }
    else
    {
      inputString += inChar;
    }  
  }
  
}
