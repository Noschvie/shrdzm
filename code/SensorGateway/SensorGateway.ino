#include "config/config.h"
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include "Ticker.h"
#include "StringSplitter.h"

extern "C" {
#include <espnow.h>
#include <user_interface.h>
}

// ESPNOW STUFF
uint8_t trigMac[6] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};
uint8_t broadcastMac[] = {0x46, 0x33, 0x33, 0x33, 0x33, 0x33};

uint8_t gatewayMac[6];

uint8_t key[16] = {0x51, 0xA0, 0xDE, 0xC5, 0x46, 0xC6, 0x77, 0xCD,
                   0x99, 0xE8, 0x61, 0xF9, 0x08, 0x77, 0x7D, 0x00
                  };

Ticker ticker, pairingTickerBlink;
int pairingCount = 0;
String deviceName;
volatile bool pairingOngoing = false;

void pairingTickerLED()
{
  if(pairingOngoing)
  {
    int state = digitalRead(LEDPIN);
    digitalWrite(LEDPIN, !state); 
  }
  else
  {
    digitalWrite(LEDPIN, HIGH);
    pairingTickerBlink.detach();
  }
}

void pairingTicker()
{
#ifdef DEBUG
    Serial.println("Pairing request sent...");
#endif

    String pl = "PAIRWITHME";

    uint8_t bs[pl.length()];
    memcpy(bs, pl.c_str(), sizeof(bs));
    esp_now_send(broadcastMac, bs, sizeof(bs));

    pairingCount++;
    if(pairingCount == 12)
    {
      Serial.println("*034[I]$"+deviceName+"$pairing:finished");
      pairingOngoing = false;
#ifdef DEBUG
      Serial.println("Pairing finished...");
#endif
      ticker.detach();
    }
}

void initVariant() 
{
  WiFi.mode(WIFI_AP);

  WiFi.macAddress(gatewayMac);

  gatewayMac[0] = 0xCE;
  gatewayMac[1] = 0x50;
  gatewayMac[2] = 0xE3;
  
  wifi_set_macaddr(SOFTAP_IF, &gatewayMac[0]);//new mac is set
}

void setup() 
{
  Serial.begin(9600);

  WiFi.macAddress(gatewayMac);

  deviceName = macToStr(gatewayMac);
  deviceName.replace(":", "");
  deviceName.toUpperCase();

  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(&broadcastMac[0], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(&trigMac[0], ESP_NOW_ROLE_CONTROLLER, 1, &key[0], 16);
  esp_now_set_peer_key(&trigMac[0], &key[0], 16);
  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
  {
    String str((char*)data);

    StringSplitter *splitter = new StringSplitter(str, '$', 3);
    int itemCount = splitter->getItemCount();
    
    if(str.substring(4,7) == "[S]")
    {
      if(itemCount == 3)
      {
/*        String setupText = "S%"+splitter->getItemAtIndex(1)+"%sensorpowerpin:14";

        uint8_t bs[setupText.length()];
        memcpy(bs, setupText.c_str(), sizeof(bs));
        esp_now_send(mac, bs, sizeof(bs)); */
      }      
    } 

    
    Serial.write(data, len);
    delay(100);
    Serial.print('\n');
    delay(100);

    delete splitter;
//    Serial.println();
    });
}

void loop() 
{
  if (Serial.available() > 0)
  {
    if ( Serial.read() == '$' ) 
    {
      String cmd = readSerial();

      if(cmd == "reset")
      {
#ifdef DEBUG
        Serial.println("Reset..");
#endif        
        ESP.restart();      
      }
      else if(cmd == "pair")
      {
        Serial.println("*033[I]$"+deviceName+"$pairing:started");

#ifdef DEBUG
        Serial.println("Pairing running...");
#endif        
        pairingCount = 0;
        pinMode(LEDPIN, OUTPUT);
        pairingOngoing = true;

        ticker.attach(5.0, pairingTicker);            
        pairingTickerBlink.attach(1.0, pairingTickerLED);            
      }
    }
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
    while (Serial.available() == 0 && counter < 20) 
    { 
      delay(1); 
      counter++;
    }
    if(counter == 20)
    {
      finished = true;
    }
    else
    {
      counter = 0;
      inByte = Serial.read();
      
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
      
  #ifdef DEBUG
  Serial.println("");  
  Serial.println("entered:"+cmd);  
  #endif     

  return cmd;
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

String split(String s, char parser, int index) 
{
  String rs="";
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) 
  {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) 
    {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex,rToIndex);
    }
    else 
      parserCnt++;
  }
  return rs;
}
