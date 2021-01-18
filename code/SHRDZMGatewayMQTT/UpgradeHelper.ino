static bool firmwareUpdate = false;   
String upgradeSSID, upgradePassword, upgradeHost, upgradeUrl;

void update_started() 
{
  DLN("CALLBACK:  HTTP update process started");
}

void update_finished() 
{
  DLN("CALLBACK:  HTTP update process finished ");
}

void update_progress(int cur, int total) 
{
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes..\n", cur, total);
}

void update_error(int err) 
{
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void executeUpdateFirmware()
{
#ifdef VERSION
  ver = String(VERSION);
#else
  ver = "0.0.0";  
#endif

#ifdef NAME
  nam = String(NAME);
#else
  nam = "SHRDZMGatewayMQTT";  
#endif 
  
  if ((WiFi.status() == WL_CONNECTED)) 
  {     
    firmwareUpdate = false;
    
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);         
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
  
    String versionStr = nam+" "+ver+" "+ESP.getSketchMD5();
    DLN("WLAN connected!");
  
    WiFiClient client; 
    Serial.printf("host:%s, url:%s, versionString:%s \n", upgradeHost.c_str(), upgradeUrl.c_str(), versionStr.c_str());
    t_httpUpdate_return ret = ESPhttpUpdate.update(upgradeHost, 80, upgradeUrl, versionStr);    
    
    switch (ret) 
    {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d):  sendUpdatedVersion %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        delay(100);
        ESP.restart();
        break;
  
      case HTTP_UPDATE_NO_UPDATES:
        DLN("HTTP_UPDATE_NO_UPDATES");
        delay(100);
        ESP.restart();
        break;
  
      case HTTP_UPDATE_OK:
        DLN("HTTP_UPDATE_OK");
        break;
    }
  }  
}

bool updateFirmware(String parameter)
{
  StringSplitter *splitter = new StringSplitter(parameter, '|', 4);
  int itemCount = splitter->getItemCount();
  bool allDataToWorkAvailable = true;

  if(itemCount == 3)  // all data from Serial
  {
    upgradeSSID = splitter->getItemAtIndex(0);
    upgradePassword = splitter->getItemAtIndex(1);
    upgradeHost = splitter->getItemAtIndex(2);
  }
  else if(itemCount == 1) // now we need to check whether SSID and password is in the configuration settings
  {
    upgradeHost = parameter;
    
    if(configuration.getWlanParameter("ssid") != "")
      upgradeSSID = configuration.getWlanParameter("ssid");
    else
      allDataToWorkAvailable = false;
      
    if(configuration.getWlanParameter("password") != "")
      upgradePassword = configuration.getWlanParameter("password");
    else
      allDataToWorkAvailable = false;    
  }

  if(!allDataToWorkAvailable)
  {
    DLN("pdateFirmware not possible due to missing parameter");
    return false;
  }

  if(upgradeHost.substring(0,7) != "http://")
  {
    DLN("Upgrade : only http addresses supported!");
    return false;    
  }    

  upgradeHost = upgradeHost.substring(7);

  if(upgradeHost.substring(upgradeHost.length()-4) != ".php")
  {
    DLN("Upgrade : only php update script supported ");
    return false;    
  }

  if(upgradeHost.indexOf('/') == -1)
  {
    DLN("Upgrade : host string not valid");
    return false;    
  }

  upgradeUrl = upgradeHost.substring(upgradeHost.indexOf('/'));
  upgradeHost = upgradeHost.substring(0,upgradeHost.indexOf('/'));

  firmwareUpdate = true;
  
  return true;
}

void OTALoop()
{
  if(firmwareUpdate)
  {
    executeUpdateFirmware();
  }
}
