//#define DEBUG
#define DEVELOPMENT
#define LEDPIN 2
#define WIFI_CHANNEL 4
#define SEND_TIMEOUT 245  // 245 millis seconds timeout 

#define SERIALBAUD 	9600

//#define MQTTGATEWAY_SUPPORT  // in this case, it runs on an ESP-12/ESP-07 and serial line is connected to a WEMOS D1

//#define HTU21D_SUPPORT
//#define DS18B20_SUPPORT
//#define DHT22_SUPPORT

#define DELAY_MESSAGE 60000

#ifdef DEVELOPMENT
uint8_t mac[] = {0x46, 0x33, 0x33, 0x33, 0x33, 0x33}; // Development
#else
uint8_t mac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33}; // Productive
#endif

#ifdef HTU21D_SUPPORT
#endif

#ifdef DS18B20_SUPPORT
#define DS18B20PIN 12     
#endif

#ifdef DHT22_SUPPORT
#define DHTPIN 12     
#define DHTTYPE DHT22   // DHT 22  or DHT11
#endif
