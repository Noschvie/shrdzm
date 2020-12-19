#ifndef CONFIG_H
#define CONFIG_H


#define VERSION		"0.3.0"
#define NAME		"SHRDZMDevice"


#define SLEEP_SECS 10
//#define SLEEP_SECS 2 * 60 
#define SEND_TIMEOUT 1000  // 245 millis seconds timeout 
#define LEDPIN 		2
#define PAIRING_PIN	13
#define SENSORPOWERPIN 14  // sensor is powerd by this pin
#define MAXCONTROLWAIT 2000	// wait 2000 millisecond in case gateway wants to control

#ifdef DEBUG_SHRDZM
#define D(...)   Serial.print (__VA_ARGS__); Serial.print(F(" "))
#define DLN(...)   Serial.print (__VA_ARGS__); Serial.println()
#define DV(...)   Serial.print(F(#__VA_ARGS__" = ")); Serial.print (__VA_ARGS__);Serial.print(F(" ")); Serial.println()
#else
#define D(...)    
#define DLN(...)  
#define DV(...)  
#endif

#include <ArduinoJson.h>
#include "SimpleEspNowConnection.h"
#include "StringSplitter.h"
#include "FS.h"
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include "..\Configuration.h"

#define SUPPORTED_DEVICES "DHT22,BH1750,BMP280,BME280,DS18B20,HTU21,HTU21D,SI7021,SHT21,MQ135,WATER,ANALOG,DIGITAL,DIGITALGROUND,RELAYTIMER,IM350,SDS011,SDS011_BMP280,SDS011_BME280,GW60"

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
#include "..\Device_GW60.h"

#endif
