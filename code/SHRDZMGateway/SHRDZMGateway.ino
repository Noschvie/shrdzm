/*
  SHRDZMGateway

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 14 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#include "SimpleEspNowConnection.h"
#include "SetupObject.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);

String inputString;
String clientAddress;

SetupObject setupObject;

void OnSendError(uint8_t* ad)
{
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
}

void OnMessage(uint8_t* ad, const char* message)
{
  Serial.println("MESSAGE:'"+String(message)+"' from "+simpleEspConnection.macToStr(ad));
}

void OnPaired(uint8_t *ga, String ad)
{
  Serial.println("EspNowConnection : Client '"+ad+"' paired! ");

  clientAddress = ad;

  // get all possible parameter
  simpleEspConnection.sendMessage("$S$", ad);  
  
}

void OnConnected(uint8_t *ga, String ad)
{
  Serial.println("EspNowConnection : Client '"+ad+"' connected! ");

  clientAddress = ad;
  SetupObject::SetupItem *si = setupObject.GetItem(ad);

  if(si != NULL)
  {
    simpleEspConnection.sendMessage((char *)si->m_parameterName.c_str(), ad);  

    setupObject.RemoveItem(si);              
  }  
  else
  {
    simpleEspConnection.sendMessage("$SLEEP$", ad);        
  }
}

void OnPairingFinished()
{
  Serial.println("Pairing finished");
}

void setup() 
{
  Serial.begin(9600);
  Serial.println();
   clientAddress = "807D3ADC8EF0"; // Test if you know the client

  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(2);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onPairingFinished(&OnPairingFinished);    
  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onConnected(&OnConnected);  

  Serial.println(WiFi.macAddress());    
}

void loop() 
{
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') 
    {
      Serial.println(inputString);

      if(inputString == "startpair")
      {
        simpleEspConnection.startPairing(30);
      }
      else if(inputString == "endpair")
      {
        simpleEspConnection.endPairing();
      }
      else if(inputString == "changepairingmac")
      {
        uint8_t np[] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};
        
        simpleEspConnection.setPairingMac(np);
      }      
      else if(inputString.substring(0,9) == "sendtest ")
      {
        simpleEspConnection.sendMessage((char *)inputString.substring(9).c_str(), clientAddress);
      }          
      else if(inputString.substring(0,9) == "posttest ")
      {
        setupObject.AddItem(clientAddress, inputString.substring(9));
      }          
      
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}
