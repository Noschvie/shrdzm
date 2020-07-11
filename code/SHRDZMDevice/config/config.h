#define VERSION		"0.2.1"
#define NAME		"SHRDZMDevice"


#define DEBUG
#define SLEEP_SECS 10
//#define SLEEP_SECS 2 * 60 
#define SEND_TIMEOUT 1000  // 245 millis seconds timeout 
#define LEDPIN 		2
#define PAIRING_PIN	13
#define SENSORPOWERPIN 14  // sensor is powerd by this pin
#define MAXCONTROLWAIT 500	// wait 500 millisecond in case gateway wants to control

#define SUPPORTED_DEVICES "DHT22,BH1750,BMP280,BME280,DS18B20,HTU21,HTU21D,SI7021,SHT21,MQ135,WATER,ANALOG,DIGITAL,DIGITALGROUND,RELAYTIMER,SDS011,GW60"

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
#include "..\Device_GW60.h"