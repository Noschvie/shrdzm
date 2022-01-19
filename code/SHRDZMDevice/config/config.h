#ifndef CONFIG_H
#define CONFIG_H

#define SMARTMETER

//#define DEBUG_SHRDZM


#define VERSION		"0.5.3"
#define NAME		"SHRDZMDevice"

//#define CloudApiAddress "http://api.shrdzm.com/shrdzm/api"
#define CloudApiAddress "https://api.shrdzm.com/shrdzm/api"
#define NTP_SERVER "at.pool.ntp.org"
#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"

#define LITTLEFS

#define SLEEP_SECS 10
#define SERIAL_BAUD 115200
#define SEND_TIMEOUT 1000  // 245 millis seconds timeout 
#define LEDPIN 		2
#define PAIRING_PIN	13
#define SENSORPOWERPIN 14  // sensor is powerd by this pin
#define MAXCONTROLWAIT 2500	// wait 2000 millisecond in case gateway wants to control
#define MQTTBufferSize 2048

#ifdef DEBUG_SHRDZM
#define SB(...)		Serial.begin(SERIAL_BAUD);  Serial.println();
#define D(...)   Serial.print (__VA_ARGS__); Serial.print(F(" "))
#define DLN(...)   Serial.print (__VA_ARGS__); Serial.println()
#define DV(...)   Serial.print(F(#__VA_ARGS__" = ")); Serial.print (__VA_ARGS__);Serial.print(F(" ")); Serial.println()
#define DEBUGGING_ENABLED true
#else
#define SB(...)
#define D(...)    
#define DLN(...)  
#define DV(...)  
#define DEBUGGING_ENABLED false
#endif

#include <ArduinoJson.h>
#include "SimpleEspNowConnection.h"
#include "../definitions.h"
#include "StringSplitter.h"
//#include <asyncHTTPrequest.h>
//#include "FS.h"

#ifdef LITTLEFS
#include <LittleFS.h>
#endif

#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <time.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
//#include <TimeLib.h>
#include "..\Configuration.h"


#ifdef SMARTMETER
#define SUPPORTED_DEVICES "IM350/AM550"
#else IM350/AM550
#define SUPPORTED_DEVICES "DHT22,BH1750,BMP280,BME280,DS18B20,HTU21,HTU21D,SI7021,SHT21,MQ135,WATER,ANALOG,DIGITAL,DIGITALGROUND,RELAYTIMER,IM350/AM550,SDS011,SDS011_BMP280,SDS011_BME280,SDS011_BH1750,GW60,DOORSENSOR"
#endif

#ifdef SMARTMETER
#include "..\Device_IM350.h"
#else	
#include "..\Device_DHT22.h"
#include "..\Device_BH1750.h"
#include "..\Device_BMP280.h"
#include "..\Device_BME280.h"
#include "..\Device_DS18B20.h"
#include "..\Device_HTU21D.h"
#include "..\Device_MQ135.h"
#include "..\Device_WATER.h"
#include "..\Device_ANALOG.h"
#include "..\Device_DIGITAL.h"
#include "..\Device_DIGITALGROUND.h"
#include "..\Device_RELAYTIMER.h"
#include "..\Device_SDS011.h"
#include "..\Device_IM350.h"
#include "..\Device_SDS011_BMP280.h"
#include "..\Device_SDS011_BME280.h"
#include "..\Device_SDS011_BH1750.h"
#include "..\Device_GW60.h"
#include "..\Device_DOORSENSOR.h"
#endif

#endif
