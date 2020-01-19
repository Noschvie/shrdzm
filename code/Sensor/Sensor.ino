#include "config/config.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>                                           // For LED status

ESP8266WebServer server(80);
bool OTAModeEnabled = false;
Ticker ticker;
bool forceReset = false;

// 5C:CF:7F:15:B7:33
uint8_t trigMac[6] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};// MAC ADDRESS OF ALL TRIG BOARDS 6 bytes
//uint8_t gatewayMac[6];

// A4:CF:12:D5:D7:67
uint8_t gatewayMac[6] = {0xCE, 0x50, 0xE3, 0xD5, 0xD7, 0x67};

uint8_t key[16] = {0x51, 0xA0, 0xDE, 0xC5, 0x46, 0xC6, 0x77, 0xCD,
                   0x99, 0xE8, 0x61, 0xF9, 0x08, 0x77, 0x7D, 0x00
                  };

extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

void initVariant() 
{
//  WiFi.softAP("SHRDZM", "135790192384", 1, 1);//actually keeps the AP non-visible, so doesn't matter too much
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &trigMac[0]);
  //Serial.println("TEST");
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
  pinMode(SETUP_PIN, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(9600); Serial.println();
#endif

  if(digitalRead(SETUP_PIN) == false)
  {
#ifdef DEBUG
  Serial.println("OTRModeEnabled = true, webserver started");

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());  
#endif

    pinMode(LEDPIN, OUTPUT);
    ticker.attach(1.0, tick);    

    uint8_t xmac[6];
    WiFi.macAddress(xmac);
    String ssid = "SHRDZM-"+macToStr(xmac);
    ssid.replace(":", "");
    ssid.toUpperCase();

    WiFi.softAP(ssid);

    server.on("/", handleRoot);
    server.begin();


 //   ticker.detach();
 //   ticker.attach(1.0, tick);

    OTAModeEnabled = true;    
  }
  else
  {
    esp_now_init();
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

void handleRoot() 
{
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void loop() 
{ 
  if(OTAModeEnabled)
  {
    server.handleClient();
  }  
}
