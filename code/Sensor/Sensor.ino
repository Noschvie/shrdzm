#include "config/config.h"
#include "SensorDataExchange.h"

#include <ESP8266WiFi.h>
#include <Ticker.h>                                           // For LED status
#include <EEPROM.h>

#ifdef DHT22_SUPPORT
#include "DHTesp.h"
#endif

#ifdef BH1750_SUPPORT
#include <BH1750FVI.h>
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
#endif

#ifdef DS18B20_SUPPORT
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(DS18B20PIN);
DallasTemperature dallas(&oneWire);
#endif

#ifdef HTU21D_SUPPORT
#include <Wire.h>
#include <HTU21D.h>
HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);
#endif

volatile bool PairingEnabled = false;
Ticker ticker;
bool forceReset = false;
volatile boolean callbackCalled;
volatile bool sendPairingInfo = false;

#ifdef DHT22_SUPPORT
DHTesp dht;
#endif

volatile int callbackCounter = 0;
volatile int sensorCount = 0;

uint8_t broadcastMac[] = {0x46, 0x33, 0x33, 0x33, 0x33, 0x33};
uint8_t trigMac[6] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};// MAC ADDRESS OF ALL TRIG BOARDS 6 bytes
uint8_t gatewayMac[6];

uint8_t key[16] = {0x51, 0xA0, 0xDE, 0xC5, 0x46, 0xC6, 0x77, 0xCD,
                   0x99, 0xE8, 0x61, 0xF9, 0x08, 0x77, 0x7D, 0x00
                  };

extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

void writeGatewayAddressToEEPROM(uint8_t *xmac)
{
  int i;

  xmac[0] = 0xCE;
  xmac[1] = 0x50;
  xmac[2] = 0xE3;

  EEPROM.begin(512);
  for(i=0;i<6;i++)
  {
    EEPROM.write(i,xmac[i]);
  }
  EEPROM.commit();
  EEPROM.end();  
}

void readGatewayAddressFromEEPROM()
{
  int i;

  EEPROM.begin(512);  
  for(i=0;i<6;i++)
  {    
    gatewayMac[i]=EEPROM.read(i);
  }  
  EEPROM.end();  
}
 
void initVariant() 
{
  WiFi.mode(WIFI_AP);

  pinMode(PAIRING_PIN, INPUT_PULLUP);

  if(digitalRead(PAIRING_PIN) == false)
  {
    wifi_set_macaddr(SOFTAP_IF, &broadcastMac[0]);
  }
  else
  {
    wifi_set_macaddr(SOFTAP_IF, &trigMac[0]);
  }
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void tick()
{
  int state = digitalRead(LEDPIN);  // get the current state of GPIO1 pin
  digitalWrite(LEDPIN, !state);     // set pin to the opposite state
}

void setup() 
{  
  pinMode(PAIRING_PIN, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(9600); Serial.println();
#endif

  if(digitalRead(PAIRING_PIN) == false)
  {
    PairingEnabled = true;    
    
#ifdef DEBUG
  Serial.println("PairingEnabled");

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());  
#endif

    pinMode(LEDPIN, OUTPUT);
    ticker.attach(0.5, tick);    

    uint8_t xmac[6];
    WiFi.macAddress(xmac);

    // listen on ESPNow SensorGateway to pair with them
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(&broadcastMac[0], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
    {
      char payload[20];
      memset(payload, 0, sizeof(payload));
      memcpy(payload, data, len);
      String pl(payload);

      Serial.println(pl);


      if(pl == "PAIRWITHME")
      {
        Serial.print("Gateway Address is ");        
        Serial.println(macToStr(mac));
        writeGatewayAddressToEEPROM(mac);
        readGatewayAddressFromEEPROM();

        sendPairingInfo = true;
      }

      PairingEnabled = false;
    });       
    
  }
  else
  {
#ifdef SENSORPOWER_SUPPORT
  pinMode(SENSORPOWERPIN,OUTPUT);
  digitalWrite(SENSORPOWERPIN,HIGH);
  #ifdef DEBUG
      Serial.println("SENSORPOWER Support enabled on port "+String(SENSORPOWERPIN));
  #endif    
#endif

    SensorDataExchange sde;

#ifdef BH1750_SUPPORT
  LightSensor.begin();  
#endif
    
#ifdef DHT22_SUPPORT
  dht.setup(DHTPIN, DHTesp::DHT22);    
  readDHT22(&sde);
#endif

#ifdef BH1750_SUPPORT
  readBH1750(&sde);
#endif

#ifdef DS18B20_SUPPORT
  readDS18B20(&sde);
#endif

#ifdef HTU21D_SUPPORT
  readHTU21D(&sde);
#endif

    sensorCount = sde.GetCount()+1;
    
    esp_now_init();

    readGatewayAddressFromEEPROM();
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(gatewayMac, ESP_NOW_ROLE_SLAVE, 1, key, 16);
    esp_now_set_peer_key(gatewayMac, key, 16);
    esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) 
    {//this is the function that is called to send data
      Serial.printf("send_cb, send done, status = %i\n", sendStatus);
      callbackCounter++;

      if(callbackCounter == sensorCount)
      {
       callbackCalled = true;
      }
    });


    for (int i = 0; i < sde.GetCount(); i++) 
    {
      String m = sde.GetMessage(i);
#ifdef DEBUG
      Serial.println(m);
#endif
      uint8_t bs[m.length()];
      memcpy(bs, m.c_str(), sizeof(bs));
      esp_now_send(gatewayMac, bs, sizeof(bs)); 
    }

    // send setupready

    //!!!!!!!!!!!!!!!!!!
    esp_now_add_peer(&trigMac[0], ESP_NOW_ROLE_CONTROLLER, 1, &key[0], 16);
    esp_now_set_peer_key(&trigMac[0], &key[0], 16);
    esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
    {
      Serial.write(data, len);
      delay(100);
      Serial.print('\n');
      delay(100);
    });
    //!!!!!!!!!!!!!!!!!!
    
    uint8_t pmac[6];
    char buffer[4];
    WiFi.macAddress(pmac);
    String deviceName = macToStr(pmac);
    deviceName.replace(":", "");
    deviceName.toUpperCase();

    String r = "[S]$"+deviceName+"$setup:ready";
    int c = r.length();
  
    sprintf(buffer, "%03d", c);
    r = "*"+String(buffer)+r;

    uint8_t bs[r.length()];
    memcpy(bs, r.c_str(), sizeof(bs));
    esp_now_send(gatewayMac, bs, sizeof(bs)); 

  }
}

void sendPairingInfoCall()
{
    wifi_set_macaddr(SOFTAP_IF, &trigMac[0]);

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(gatewayMac, ESP_NOW_ROLE_SLAVE, 1, key, 16);
    esp_now_set_peer_key(gatewayMac, key, 16);
    esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) 
    {//this is the function that is called to send data
      Serial.printf("pairing info sent with status = %i\n", sendStatus);

      PairingEnabled = false;
      callbackCalled = true;
    });  

    uint8_t pmac[6];
    char buffer[4];
    WiFi.macAddress(pmac);
    String deviceName = macToStr(pmac);
    deviceName.replace(":", "");
    deviceName.toUpperCase();

    String r = "[P]$"+deviceName+"$paired:OK";
    int c = r.length();
  
    sprintf(buffer, "%03d", c);
    r = "*"+String(buffer)+r;
  
    uint8_t bs[r.length()];
    memcpy(bs, r.c_str(), sizeof(bs));
    esp_now_send(gatewayMac, bs, sizeof(bs));
}

void loop() 
{ 
  if(PairingEnabled)
  {
    return;
  }
  else if(sendPairingInfo)
  {
    Serial.println("will send pairing info now");        

    PairingEnabled = true;
    sendPairingInfoCall();
    
    delay(100);
    ESP.restart();
  }


  if (callbackCalled || (millis() > SEND_TIMEOUT)) 
  {
    gotoSleep();
  }  
}

void readHTU21D(SensorDataExchange *sde)
{
#ifdef HTU21D_SUPPORT  
  myHTU21D.begin();
  
  sde->AddSensorData("humidity", String(myHTU21D.readHumidity()));
  sde->AddSensorData("temperature", String(myHTU21D.readTemperature()));  
#endif
}

void readDS18B20(SensorDataExchange *sde)
{
#ifdef DS18B20_SUPPORT
  dallas.begin();
  DeviceAddress thermometerAddress;


  for (int i = 0; i < dallas.getDeviceCount(); i++) 
  {
    if(dallas.getAddress(thermometerAddress, i))
    {
      dallas.setResolution(thermometerAddress, 12);
    }
  }

  dallas.requestTemperatures();

  for (int i = 0; i < dallas.getDeviceCount(); i++) 
  {
      if(dallas.getAddress(thermometerAddress, i))
      {        
        sde->AddSensorData("temperature_"+getAddressString(thermometerAddress), String(dallas.getTempC(thermometerAddress)));
      }
  }
  
#endif
}

#ifdef DS18B20_SUPPORT
String getAddressString(DeviceAddress thermometerAddress)
{
  String a;

  for(int i = 5; i<8; i++)
  {
    if( thermometerAddress[i] < 16)
      a+= "0";
    a+= String(thermometerAddress[i], HEX);
  }

  return a;
}
#endif

void readDHT22(SensorDataExchange *sde)
{ 
#ifdef DHT22_SUPPORT
  delay(dht.getMinimumSamplingPeriod());

  sde->AddSensorData("temperature", String(dht.getTemperature()));
  sde->AddSensorData("humidity", String(dht.getHumidity()));
#endif
}

#ifdef BH1750_SUPPORT
void readBH1750(SensorDataExchange *sde)
{
  sde->AddSensorData("illuminance", String(LightSensor.GetLightIntensity()));
}
#endif

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
  delay(100);
}
