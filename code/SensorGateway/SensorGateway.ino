#include "config/config.h"

#include <ESP8266WiFi.h>

#ifdef HTU21D_SUPPORT
#include <HTU21D.h>
#include <Wire.h>
#endif

#ifdef DHT22_SUPPORT
//#include "DHT.h"
#include "DHTesp.h"
#endif

#ifdef DS18B20_SUPPORT
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

extern "C" {
  #include <espnow.h>
  #include "user_interface.h"
}

void initVariant() 
{
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &mac[0]);
}

struct __attribute__((packed)) SENSOR_DATA {
    char mac[18];
    float temp;
    float humidity;
    float battery;
    bool batteryState;
} sensorData;

unsigned long lastMsg = 0;
unsigned long delayMS = DELAY_MESSAGE;
String localmac = "";

#ifdef DS18B20_SUPPORT
OneWire  oneWire(DS18B20PIN);  // a 4.7K resistor is necessary
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer, outsideThermometer;
#endif
#ifdef DHT22_SUPPORT
DHTesp dht;
//DHT dht(DHTPIN, DHTTYPE);
#endif
#ifdef HTU21D_SUPPORT
HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);
#endif


void setup() 
{
#ifdef SERIALBAUD
  Serial.begin(SERIALBAUD); 
#else  
  Serial.begin(9600); 
#endif

  Serial.flush();

#ifdef LEDPIN
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);        
#endif

  localmac = WiFi.macAddress();
  localmac.replace(":","");


  //initVariant();

#ifdef DEBUG
  Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("This node STA mac: "); Serial.println(WiFi.macAddress());
#endif

  lastMsg = millis();

#ifdef HTU21D_SUPPORT
  myHTU21D.begin();
#endif
#ifdef DHT22_SUPPORT
  //dht.begin();
  dht.setup(DHTPIN, DHTesp::DHT22);  
#endif

  initEspNow();  

#ifdef DEBUG
  Serial.printf("This mac: %s, ", localmac.c_str()); 
  Serial.printf("target mac: %02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); 
  Serial.printf(", channel: %i\n", WIFI_CHANNEL); 
#endif

  delay(500);
  
#if defined(DHT22_SUPPORT) || defined(HTU21D_SUPPORT) || defined (DS18B20_SUPPORT)
  updateOnboardData();
#endif
}

void loop() 
{
#if defined(DHT22_SUPPORT) || defined(HTU21D_SUPPORT) || defined (DS18B20_SUPPORT)
  unsigned long now = millis();
  
  if (now - lastMsg > delayMS) 
  {
    updateOnboardData();        
    lastMsg = now;
  }
#endif  

  if (Serial.available() > 0)
  {
    if ( Serial.read() == '$' ) 
    {
      String cmd = readSerial();

      if(cmd == "reset")
      {
#ifdef DEBUG
        Serial.println("Reset..");
#endif        
        ESP.restart();      }
    }
  }
}

String readSerial() 
{
  String cmd = "";
  byte inByte = 0;
  int counter = 0;
  bool finished = false;

  while (!finished)
  {
    while (Serial.available() == 0 && counter < 20) 
    { 
      delay(1); 
      counter++;
    }
    if(counter == 20)
    {
      finished = true;
    }
    else
    {
      counter = 0;
      inByte = Serial.read();
      
      if (inByte == '\n')
      {
        finished = true;
      }
      else
      {
        cmd += (char)inByte;
      }
    }    
  }
      
  #ifdef DEBUG
  Serial.println("");  
  Serial.println("entered:"+cmd);  
  #endif     

  return cmd;
}

void initEspNow() 
{
  if (esp_now_init()!=0) 
  {
    #ifdef DEBUG
    Serial.println("*** ESP_Now init failed");  
    #endif
    
    ESP.restart();
  }
  else
  {
    #ifdef DEBUG
    Serial.println("*** ESP_Now successfully initialized");  
    #endif
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
  {
#ifdef LEDPIN
      digitalWrite(LEDPIN, LOW);      
#endif
    
    memcpy(&sensorData, data, sizeof(sensorData));

    String m = String(sensorData.mac);
    m.replace(":","");

#ifdef MQTTGATEWAY_SUPPORT    
    Serial.print("$$");
    Serial.write("01");
    Serial.write(sizeof(sensorData));
    Serial.write((uint8_t *)&sensorData, sizeof(sensorData));  
#else
    Serial.println("S:"+m+" T:"+String(sensorData.temp)+
                  " H:"+String(sensorData.humidity)+
                  " BV:"+String(sensorData.battery)+
                  " BS:"+String((sensorData.batteryState == true) ? "OK" : "NOK"));
    Serial.flush();
#endif
    delay(100);                  
#ifdef LEDPIN
      digitalWrite(LEDPIN, HIGH);      
#endif
  });  
}

void updateOnboardData()
{
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

  
#ifdef MQTTGATEWAY_SUPPORT    
    uint8_t macX = 0;
    Serial.print("$$");
    Serial.write("01"); // mac address of remote ESP-Now device
    Serial.write(sizeof(sensorData));
    Serial.write((uint8_t *)&sensorData, sizeof(sensorData));  
 #else
  Serial.println("S:"+localmac+" T:"+String(sensorData.temp)+
                " H:"+String(sensorData.humidity)+
                " BV:"+String(sensorData.battery)+
                " BS:"+String((sensorData.batteryState == true) ? "OK" : "NOK"));
    Serial.flush();                
#endif

}

void readHTU21D()
{
#ifdef HTU21D_SUPPORT
  sensorData.humidity = myHTU21D.readHumidity();
  sensorData.temp = myHTU21D.readTemperature();
  sensorData.battery = 0.0f;  
  sensorData.batteryState = myHTU21D.batteryStatus();
#endif
}

bool readDHT22()
{
#ifdef DHT22_SUPPORT
  delay(dht.getMinimumSamplingPeriod());

  sensorData.humidity = dht.getHumidity();
  sensorData.temp = dht.getTemperature();
    
/*  sensorData.humidity = dht.readHumidity();
  sensorData.temp = dht.readTemperature(); */
  sensorData.battery = 0.0f;  
  sensorData.batteryState = "OK";

  return (dht.getStatus() == 0) ? true : false;
#endif
}

void readDS18B20()
{
#ifdef DS18B20_SUPPORT
  DeviceAddress insideThermometer;
  
  sensors.begin();
  sensors.getAddress(insideThermometer, 0);
  sensors.setResolution(12);
  sensors.requestTemperatures();
  
  sensorData.humidity = 0.0f;
  sensorData.temp = (float)sensors.getTempC(insideThermometer);  
  sensorData.battery = 0.0f;  
  sensorData.batteryState = "OK";
#endif
}
