
// ESPNOW STUFF
uint8_t trigMac[6] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};

uint8_t broadcastMac[] = {0x46, 0x33, 0x33, 0x33, 0x33, 0x33};

// A4:CF:12:D5:D7:67
uint8_t gatewayMac[6];
//uint8_t gatewayMac[6] = {0xCE, 0x50, 0xE3, 0xD5, 0xD7, 0x67};


uint8_t key[16] = {0x51, 0xA0, 0xDE, 0xC5, 0x46, 0xC6, 0x77, 0xCD,
                   0x99, 0xE8, 0x61, 0xF9, 0x08, 0x77, 0x7D, 0x00
                  };

bool PairingEnabled = false;
//bool PairingEnabled = true;


extern "C" {
#include <espnow.h>
#include <user_interface.h>
}

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino


void initVariant() 
{
  WiFi.mode(WIFI_AP);

  WiFi.macAddress(gatewayMac);
  gatewayMac[0] = 0xCE;
  gatewayMac[1] = 0x50;
  gatewayMac[2] = 0xE3;
  
  wifi_set_macaddr(SOFTAP_IF, &gatewayMac[0]);//new mac is set
}

void setup() {
  Serial.begin(9600);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());//take this and use it to modify the mac address above

  if(PairingEnabled)
  {
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(&broadcastMac[0], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
    {
      Serial.write(data, len);
      Serial.println("");
    });

    char payload[20];//limit is liek 200bytes, but we don't need anything close to that
    sprintf(payload, "PAIRWITHME");

    uint8_t bs[strlen(payload)];
    memcpy(bs, &payload, strlen(payload));
    esp_now_send(broadcastMac, bs, strlen(payload));
    
  }
  else
  {
    // setup ESPNOW as receiver
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_add_peer(&trigMac[0], ESP_NOW_ROLE_CONTROLLER, 1, &key[0], 16);
    esp_now_set_peer_key(&trigMac[0], &key[0], 16);
    esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) 
    {
      Serial.write(data, len);
      Serial.println("");
    });
  }
}

void loop() 
{
//wow that was easy!!! nothing here... well that ESPnow function up there gets called when new data shows up.  Best to not do anything here anyway, since we don't want to miss anything
}
