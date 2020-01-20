#include "config/config.h"

#include <ESP8266WiFi.h>
#include <Ticker.h>                                           // For LED status
#include <EEPROM.h>

bool PairingEnabled = false;
Ticker ticker;
bool forceReset = false;

uint8_t broadcastMac[] = {0x46, 0x33, 0x33, 0x33, 0x33, 0x33};


// 5C:CF:7F:15:B7:33
uint8_t trigMac[6] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};// MAC ADDRESS OF ALL TRIG BOARDS 6 bytes
uint8_t* gatewayMac;
//uint8_t gatewayMac[6];

// A4:CF:12:D5:D7:67
//uint8_t gatewayMac[6] = {0xCE, 0x50, 0xE3, 0xD5, 0xD7, 0x67};

uint8_t key[16] = {0x51, 0xA0, 0xDE, 0xC5, 0x46, 0xC6, 0x77, 0xCD,
                   0x99, 0xE8, 0x61, 0xF9, 0x08, 0x77, 0x7D, 0x00
                  };

extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

void writeGatewayAddressToEEPROM(uint8_t *xmac)
{
  int i;

  xmac[0] = 0xCE;
  xmac[1] = 0x50;
  xmac[2] = 0xE3;

  for(i=0;i<6;i++)
  {
    EEPROM.write(i,xmac[i]);
  }
  EEPROM.commit();
}

uint8_t* readGatewayAddressFromEEPROM()
{
  int i;
  uint8_t xmac[6];

  for(i=0;i<6;i++)
  {    
    xmac[i]=EEPROM.read(i);
  }  

  return xmac;
}
 
void initVariant() 
{
  WiFi.mode(WIFI_AP);

  pinMode(PAIRING_PIN, INPUT_PULLUP);

  if(digitalRead(PAIRING_PIN) == false)
  {
    wifi_set_macaddr(SOFTAP_IF, &broadcastMac[0]);
  }
  else
  {
    wifi_set_macaddr(SOFTAP_IF, &trigMac[0]);
  }
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

void tick()
{
  int state = digitalRead(LEDPIN);  // get the current state of GPIO1 pin
  digitalWrite(LEDPIN, !state);     // set pin to the opposite state
}

void setup() 
{
  EEPROM.begin(512);
  
  pinMode(PAIRING_PIN, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(9600); Serial.println();
#endif

  if(digitalRead(PAIRING_PIN) == false)
  {
    Serial.println("Stored GatewayAddress from EEPROM : "+macToStr(readGatewayAddressFromEEPROM()));
    
/*    uint8_t  MAC_softAP[]          = {0,0,0,0,0,0}; 
    uint8_t* MAC  = WiFi.softAPmacAddress(MAC_softAP);                   //get MAC address of softAP interface
    for (int i = 0; i < sizeof(MAC)+2; ++i){                                                          //this line needs cleaning up.
         Serial.print(":");
         Serial.print(MAC[i],HEX);
         MAC_softAP[i] = MAC[i];                                         //copy back to global variable
    } */
    
#ifdef DEBUG
  Serial.println("PairingEnabled");

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());  
#endif

    pinMode(LEDPIN, OUTPUT);
    ticker.attach(1.0, tick);    

    uint8_t xmac[6];
    WiFi.macAddress(xmac);
    String deviceName = macToStr(xmac);
    deviceName.replace(":", "");
    deviceName.toUpperCase();

    // listen on ESPNow SensorGateway to pair with them
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(&broadcastMac[0], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
    {
      char payload[20];
      memcpy(payload, data, len);
      String pl(payload);

      if(pl == "PAIRWITHME")
      {
        Serial.println("Gateway Address is "+macToStr(mac));
        writeGatewayAddressToEEPROM(mac);
      }

      delay(100);
      ESP.restart();
    });       

 //   ticker.detach();
 //   ticker.attach(1.0, tick);

    PairingEnabled = true;    
  }
  else
  {
    esp_now_init();

    gatewayMac = readGatewayAddressFromEEPROM();
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_add_peer(gatewayMac, ESP_NOW_ROLE_SLAVE, 1, key, 16);
    esp_now_set_peer_key(gatewayMac, key, 16);
    esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) {//this is the function that is called to send data
      Serial.printf("send_cb, send done, status = %i\n", sendStatus);
    });

    char payload[100];//limit is liek 200bytes, but we don't need anything close to that
    sprintf(payload, "TEST");

    uint8_t bs[strlen(payload)];
    memcpy(bs, &payload, strlen(payload));
    esp_now_send(gatewayMac, bs, strlen(payload));
    
  }
}

void loop() 
{ 
}
