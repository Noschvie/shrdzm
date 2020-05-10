/*
  SHRDZMGateway

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 05 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#include "SimpleEspNowConnection.h"
#include "SetupObject.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);

String inputString;
String clientAddress;

SetupObject setupObject;


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
    simpleEspConnection.sendMessage((char *)si->m_parameterName.c_str(), clientAddress);  

    setupObject.RemoveItem(si);              
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
  // clientAddress = "ECFABC0CE7A2"; // Test if you know the client

  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(2);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onPairingFinished(&OnPairingFinished);    
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
        
//        simpleEspConnection.sendMessage((char *)inputString.substring(9).c_str(), clientAddress);
      }          
      
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}
