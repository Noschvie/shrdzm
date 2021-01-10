#define VERSION		"0.4.0"
#define NAME		"SHRDZMGateway"


#define LEDPIN 2
//#define LITTLEFS

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

#include "SimpleEspNowConnection.h"

#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include "StringSplitter.h"


#include "..\Configuration.h"

#ifdef LITTLEFS
#include <LittleFS.h>
#endif