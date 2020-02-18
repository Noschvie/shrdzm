#define DEBUG
#define WIFI_CHANNEL 4
#define SLEEP_SECS 2 * 60 
#define SEND_TIMEOUT 1000  // 245 millis seconds timeout 
#define SETUPWAIT_TIMEOUT 200 // will be added to SEND_TIMEOUT
#define LEDPIN 		2
#define PAIRING_PIN	13

#define SENSORPOWER_SUPPORT

#define BH1750_SUPPORT
#define DS18B20_SUPPORT
//#define DHT22_SUPPORT
#define HTU21D_SUPPORT

#ifdef DS18B20_SUPPORT
#define DS18B20PIN 12     
#endif

#ifdef DHT22_SUPPORT
#define DHTPIN 12     
#define DHTTYPE DHT22   // DHT 22  or DHT11
#endif

#ifdef SENSORPOWER_SUPPORT
#define SENSORPOWERPIN 14  // sensor is powerd by this pin
#endif

