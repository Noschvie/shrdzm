#ifndef CONFIG_H
#define CONFIG_H

#define VERSION		"0.5.1"
#define NAME		"SHRDZMGatewayMQTT"

#define CloudApiAddress "https://api.shrdzm.com/shrdzm/api"
#define NTP_SERVER "at.pool.ntp.org"
#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"

//#define LITTLEFS

#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "StringSplitter.h"
#include <MQTT.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include <RCSwitch.h>
#include <Ticker.h>
#include <time.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#ifdef LITTLEFS
#include <LittleFS.h>
#endif

#include "..\Configuration.h"
#include "..\definitions.h"


#ifdef DEBUG_SHRDZM
#define D(...)   Serial.print (__VA_ARGS__); Serial.print(F(" "))
#define DLN(...)   Serial.print (__VA_ARGS__); Serial.println()
#define DV(...)   Serial.print(F(#__VA_ARGS__" = ")); Serial.print (__VA_ARGS__);Serial.print(F(" ")); Serial.println()
#else
#define D(...)    
#define DLN(...)  
#define DV(...)  
#endif


#define DEBUG
#define LEDPIN 2
#define RESET_PIN 16
#define CONFIG_PIN 13

//#define SERIALBAUD 	115200
#define SERIALBAUD 	9600

#define RCSWITCHPIN 0
#define RCSENDPIN	15

#endif