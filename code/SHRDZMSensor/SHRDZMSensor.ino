/*
  SHRDZMSensor

  Created 05 Mai 2020
  By Erich O. Pintar
  Modified 05 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly

*/

#include <FS.H>
#include <ArduinoJson.h>
#include "config/config.h"

#include "SimpleEspNowConnection.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);

String inputString;
String serverAddress;

void OnMessage(uint8_t* ad, const char* message)
{
  Serial.println("MESSAGE:"+String(message));
}

void OnNewGatewayAddress(uint8_t *ga, String ad)
{  
  Serial.println("New GatewayAddress '"+ad+"'");

  simpleEspConnection.setServerMac(ga);
}

void setup() 
{
#ifdef DEBUG
  Serial.begin(9600); Serial.println();
#endif

  pinMode(PAIRING_PIN, INPUT_PULLUP);

  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(LEDPIN);  

  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onMessage(&OnMessage);  

  if(digitalRead(PAIRING_PIN) == false)
  {
    simpleEspConnection.startPairing(300);
  }
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
      else if(inputString == "sendtest")
      {
         simpleEspConnection.sendMessage("This comes from the Client");
      }
      
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}
