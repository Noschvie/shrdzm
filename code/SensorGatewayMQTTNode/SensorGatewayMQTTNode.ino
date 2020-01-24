#include "config/config.h"

#include <SoftwareSerial.h>
#include "StringSplitter.h"

SoftwareSerial swSer;


void setup() 
{  
#ifdef DEBUG  
  Serial.begin(9600);
#endif  

#ifdef SERIALBAUD
  swSer.begin(SERIALBAUD, SWSERIAL_8N1, 14, 12, false);  
#else
  swSer.begin(9600, SWSERIAL_8N1, 14, 12, false, 95, 11);
#endif

  
}

void loop() 
{

  if (swSer.available() > 0)
  {
    String cmd = readSerial();

#ifdef DEBUG
    Serial.println("Command : "+cmd);
#endif        

    StringSplitter *splitter = new StringSplitter(cmd, '$', 5);
    int itemCount = splitter->getItemCount();
  

    for(int i = 0; i < itemCount; i++)
    {
      String item = splitter->getItemAtIndex(i);
      
      Serial.println(String(i)+":"+item);
    }    

    delete splitter;
  }    

  
  
}

String readSerial() 
{
  String cmd = "";
  byte inByte = 0;
  int counter = 0;
  bool finished = false;
  
  while (!finished)
  {
    while (swSer.available() > 0)     
    {
      counter = 0;
      inByte = swSer.read();
      
      if (inByte == '\n')
      {
        finished = true;
      }
      else
      {
        cmd += (char)inByte;
      }
    }    
    yield();
  }  

  return cmd;
}
