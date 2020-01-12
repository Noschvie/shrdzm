#include "config/config.h"

#include <ESP8266WiFi.h>

extern "C" {
  #include <espnow.h>
}

#ifdef DS18B20_SUPPORT
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

#ifdef DHT22_SUPPORT
//#include "DHT.h"
#include "DHTesp.h"
#endif

#ifdef HTU21D_SUPPORT
#include <Wire.h>
#include <HTU21D.h>
#endif


struct __attribute__((packed)) SENSOR_DATA {
    char mac[18];
    float temp;
    float humidity;
    float battery;
    bool batteryState;
} sensorData;

#ifdef DS18B20_SUPPORT
OneWire  oneWire(DS18B20PIN);  // a 4.7K resistor is necessary
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer, outsideThermometer;
#endif
#ifdef DHT22_SUPPORT
//DHT dht(DHTPIN, DHTTYPE);
DHTesp dht;
#endif
#ifdef HTU21D_SUPPORT
HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);
#endif

volatile boolean callbackCalled;

void setup() 
{
  sensorData.batteryState = "OK";
#ifdef DEBUG
  Serial.begin(9600); Serial.println();
#endif

#ifdef SENSORPOWER_SUPPORT
  pinMode(SENSORPOWERPIN,OUTPUT);
  digitalWrite(SENSORPOWERPIN,HIGH);
  #ifdef DEBUG
      Serial.println("SENSORPOWER Support enabled on port "+String(SENSORPOWERPIN));
  #endif    
#endif

#ifdef ATTINY_SUPPORT
  pinMode(ATTINYPIN, OUTPUT);
  #ifdef DEBUG
      Serial.println("ATTINY Support enabled on port "+String(ATTINYPIN));
  #endif    
#endif

#ifdef DHT22_SUPPORT
  dht.setup(DHTPIN, DHTesp::DHT22);    
#endif
#ifdef HTU21D_SUPPORT
  if(myHTU21D.begin() == false)
  {
    sensorData.batteryState = "NOK";    
  }
#endif

  if (esp_now_init() != 0) 
  {
#ifdef DEBUG
    Serial.println("*** ESP_Now init failed");
#endif    
    gotoSleep();
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(remoteMac, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, NULL, 0);

  esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) 
  {
#ifdef DEBUG
    Serial.printf("send_cb, send done, status = %i\n", sendStatus);
#endif
    callbackCalled = true;
  });

  callbackCalled = false;

  String mac = WiFi.macAddress().c_str();
  mac.toCharArray(sensorData.mac,mac.length()+1);

#ifdef DS18B20_SUPPORT
  readDS18B20();
#endif
#ifdef DHT22_SUPPORT  
  if(!readDHT22())
    return;
#endif
#ifdef HTU21D_SUPPORT
  readHTU21D();
#endif

#ifdef DEBUG
  Serial.printf("This mac: %s, ", sensorData.mac); 
  Serial.printf("target mac: %02x%02x%02x%02x%02x%02x", remoteMac[0], remoteMac[1], remoteMac[2], remoteMac[3], remoteMac[4], remoteMac[5]); 
  Serial.printf(", channel: %i\n", WIFI_CHANNEL); 
  Serial.println("T : "+String(sensorData.temp)+" H : "+String(sensorData.humidity));
#endif

  uint8_t bs[sizeof(sensorData)];
  memcpy(bs, &sensorData, sizeof(sensorData));
  esp_now_send(NULL, bs, sizeof(sensorData)); // NULL means send to all peers
}

void loop() 
{
  if (callbackCalled || (millis() > SEND_TIMEOUT)) 
  {
    gotoSleep();
  }
}

void gotoSleep() 
{  
  int sleepSecs = SLEEP_SECS; 
#ifdef DEBUG
  Serial.printf("Up for %i ms, going to sleep for %i secs...\n", millis(), sleepSecs); 
#endif

#ifdef ATTINY_SUPPORT
  digitalWrite(ATTINYPIN, HIGH);
  #ifdef DEBUG
      Serial.println("Port "+String(ATTINYPIN)+" set to HIGH");
  #endif    
  //delay(1000);
  digitalWrite(ATTINYPIN, LOW);
  //pinMode(ATTINYPIN, INPUT);
#endif

  ESP.deepSleep(sleepSecs * 1000000, RF_NO_CAL);
}

void readDS18B20()
{
#ifdef DS18B20_SUPPORT
  DeviceAddress insideThermometer;
  
  sensors.begin();
  sensors.getAddress(insideThermometer, 0);
  sensors.setResolution(12);
  sensors.requestTemperatures();
  
  sensorData.humidity = 999;
  sensorData.temp = (float)sensors.getTempC(insideThermometer);  
#endif
}

bool readDHT22()
{
#ifdef DHT22_SUPPORT
  delay(dht.getMinimumSamplingPeriod());

  sensorData.humidity = dht.getHumidity();
  sensorData.temp = dht.getTemperature();

  sensorData.battery = 0.0f;  
  sensorData.batteryState = "OK";

  return (dht.getStatus() == 0) ? true : false;
#endif
}

void readHTU21D()
{
#ifdef HTU21D_SUPPORT
  sensorData.humidity = myHTU21D.readHumidity();
  sensorData.temp = myHTU21D.readTemperature();
  sensorData.batteryState = myHTU21D.batteryStatus();
#endif
}
