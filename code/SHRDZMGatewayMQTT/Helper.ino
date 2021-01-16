void setDeviceName()
{
  uint8_t pmac[6];
  WiFi.macAddress(pmac);
  deviceName = macToStr(pmac);

  deviceName.replace(":", "");
  deviceName.toUpperCase();
}

String getUptime()
{
  char uptime[50];
  long millisecs = millis();

  snprintf(uptime, 50, "%d Days, %02d Hours, %02d Minutes",
    int(millisecs / ((1000*60*60*24)) % 365),
    int(millisecs / ((1000*60*60)) % 24),
    int(millisecs / ((1000*60)) % 60));

  return uptime;  
}

String macToStr(const uint8_t* mac)
{
  char mac_addr[13];
  mac_addr[12] = 0;
  
  sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

  return String(mac_addr);
}
