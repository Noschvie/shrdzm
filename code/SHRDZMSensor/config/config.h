#define DEBUG
#define WIFI_CHANNEL 4
#define SLEEP_SECS 2 * 60 
#define SEND_TIMEOUT 1000  // 245 millis seconds timeout 
#define LEDPIN 		2
#define PAIRING_PIN	13
#define SENSORPOWERPIN 14  // sensor is powerd by this pin


#define DEVICETYPE_DHT22
#define DEVICETYPE_BH1750
#define DEVICETYPE_DS18B20
#define DEVICETYPE_HTU21D
#define DEVICETYPE_WATERSENSOR

#ifdef DEVICETYPE_DHT22
	#define DHT22_PIN	12
#endif
