#define DEBUG
#define DEVELOPMENT
#define WIFI_CHANNEL 4
#define SLEEP_SECS 2 * 60 
#define SEND_TIMEOUT 245  // 245 millis seconds timeout 

//#define DS18B20_SUPPORT
#define DHT22_SUPPORT
//#define HTU21D_SUPPORT

//#define ATTINY_SUPPORT
#define SENSORPOWER_SUPPORT

#ifdef DEVELOPMENT
uint8_t remoteMac[] = {0x46, 0x33, 0x33, 0x33, 0x33, 0x33}; // Development
#else
uint8_t remoteMac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33}; // Productive
#endif

#ifdef DS18B20_SUPPORT
#define DS18B20PIN 12     
#endif

#ifdef DHT22_SUPPORT
#define DHTPIN 12     
#define DHTTYPE DHT22   // DHT 22  or DHT11
#endif

#ifdef SENSORPOWER_SUPPORT
#define SENSORPOWERPIN 4
#endif

#ifdef ATTINY_SUPPORT
#define ATTINYPIN 5
#endif
