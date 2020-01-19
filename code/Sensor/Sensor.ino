#include "config/config.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>                                           // For LED status

ESP8266WebServer server(80);
bool OTAModeEnabled = false;
Ticker ticker;
bool forceReset = false;

extern "C" {
  #include <espnow.h>
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
