#define VERSION		"0.5.1"
#define NAME		"SHRDZMGateway"


//#define DEBUG
#define LEDPIN 2
#define WIFI_CHANNEL 4
#define SEND_TIMEOUT 1000  // 245 millis seconds timeout 

#define ACCESSPOINT_PIN	13

#define SERIALBAUD 	9600

#ifdef DEBUG_SHRDZM
#define D(...)   Serial.print (__VA_ARGS__); Serial.print(F(" "))
#define DLN(...)   Serial.print (__VA_ARGS__); Serial.println()
#define DV(...)   Serial.print(F(#__VA_ARGS__" = ")); Serial.print (__VA_ARGS__);Serial.print(F(" ")); Serial.println()
#else
#define D(...)    
#define DLN(...)  
#define DV(...)  
#endif


#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFiMulti.h>