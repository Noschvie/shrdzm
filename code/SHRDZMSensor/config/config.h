#define DEBUG
#define SLEEP_SECS 2 * 60 
#define SEND_TIMEOUT 1000  // 245 millis seconds timeout 
#define LEDPIN 		2
#define PAIRING_PIN	13
#define SENSORPOWERPIN 14  // sensor is powerd by this pin
#define MAXCONTROLWAIT 500	// wait 500 millisecond in case gateway wants to control

#include "..\Device_DHT22.h"
#include "..\Device_BH1750.h"
#include "..\Device_DS18B20.h"
#include "..\Device_HTU21D.h"
#include "..\Device_Watersensor.h"
