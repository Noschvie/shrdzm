void changeConfigurationBlinker()
{
#ifdef LEDPIN
  digitalWrite(LEDPIN, !(digitalRead(LEDPIN)));
#endif
}

char* getWebsite(char* content, bool update = false)
{
  int len = strlen(content);

  DLN("Content len = "+String(len));

  sprintf(websideBuffer,  
    website_template,
    update ? F("<script src=\"j.js\"></script>") : F(""), 
    deviceName.c_str(), deviceName.c_str(), content);

  return websideBuffer;
}
void handleJson(AsyncWebServerRequest *request) 
{
  // Output: send data to browser as JSON
  String message = "";

  time(&now);
  localtime_r(&now, &tm);

  char t[22];
  sprintf(t, "%4d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );

  message += (F("{\"mqttconnectionstate\":"));
  message += (mqttclient.connected() ? String("\"Connected\"") : String("\"Not Connected\""));  
  message += (F(",\"lastmessage\":\""));
  message += lastMessage+"\"";  
  message += (F(",\"timestamp\":\""));
  message += String(t);  
  message += (F("\"}")); // End of JSON
 
  request->send(200, "application/json", message);

  //DLN(message);
}

void handleJs(AsyncWebServerRequest *request)
{  
  String message;
  message += F("const url ='json';\n"
               "function renew(){\n"
               " fetch(url)\n" // Call the fetch function passing the url of the API as a parameter
               " .then(response => {return response.json();})\n"
               " .then(jo => {\n"
               "   for (var i in jo)\n"
               "    {if (document.getElementById(i))\n"
               "      {\n"
               "          document.getElementById(i).innerHTML = jo[i];}\n"              
               "      }\n"
               " })\n"
               " .catch(function() {\n"                                        // this is where you run code if the server returns any errors
               " });\n"
               "}\n"
               "document.addEventListener('DOMContentLoaded', renew, setInterval(renew, ");
  message += ajaxIntervall * 1000;
  message += F("));");

  request->send(200, "text/javascript", message);
}

void handleRoot(AsyncWebServerRequest *request) 
{
  if(request->hasArg("factoryreset"))
  {
    if(String(request->arg("factoryreset")) == "true") // factory reset was pressed
    {
      snprintf(menuContextBuffer, 300,
      "<!DOCTYPE html>\
      <html>\
      <head>\
      </head>\
      <body>\
      <h1>Factory Reset was pressed. SHRDZMGatewayMQTT will be restarted with default configuration.</h1>\
      </body>\
      </html>\
      ");
        
      request->send(200, F("text/html"), menuContextBuffer);

      configuration.resetConfiguration();

      delay(2000);
      
      ESP.restart();        
    }
  }

  if(request->hasArg("upgradeMQTT") && request->hasArg("upgradepathMQTT"))
  {
    if(String(request->arg("upgradeMQTT")) == "true") // Upgrade was pressed
    {
      DLN("Upgrade from "+String(request->arg("upgradepathMQTT"))+" will be started...");
      //if(updateFirmwareByMQTT(server.arg("upgradepathMQTT")))
      if(updateFirmware(request->arg("upgradepathMQTT")))
      {        
        snprintf(menuContextBuffer, 300,
        "<!DOCTYPE html>\
        <html>\
        <head>\
        </head>\
        <body>\
        <h1>Upgrade started. SHRDZMGatewayMQTT will reboot after upgrade. </h1>\
        </body>\
        </html>\
        ");
          
        request->send(200, "text/html", menuContextBuffer);  
        return;      
      }
    }    
  }  

  if(request->hasArg("upgradeESPNow") && request->hasArg("upgradepathESPNow"))
  {
    if(String(request->arg("upgradeESPNow")) == "true") // Upgrade was pressed
    {
          String upgradeText = String("$upgrade "+WiFi.SSID()+"|"+WiFi.psk()+"|"+request->arg("upgradepathESPNow"));
          
          DLN("Send upgrade of GATEWAY called : '"+upgradeText+"'");

          swSer.write(upgradeText.c_str());
          swSer.write('\n');           
      
          snprintf(menuContextBuffer, 300,
          "<!DOCTYPE html>\
          <html>\
          <head>\
          </head>\
          <body>\
          <h1>Upgrade started. SHRDZMGateway will reboot after upgrade.</h1>\
          </body>\
          </html>\
          ");
            
          request->send(200, F("text/html"), menuContextBuffer);  
          return;      
    }    
  }  

  sprintf(menuContextBuffer,  
      handleRoot_template,
      ver.c_str(),
      String(ESP.getChipId()).c_str(),
      ESP.getSketchMD5().c_str(),
      subscribeTopicSet.c_str(),
      subscribeTopicConfig.c_str(),
      subscribeTopicRCSEND.c_str(),
      String(MQTT_TOPIC).c_str(), 
      String(configuration.getCloudParameter("enabled")) == "true" ? "Enabled" : "Disabled",      
      WiFi.localIP().toString().c_str(),   
      WiFi.dnsIP().toString().c_str(), 
      WiFi.gatewayIP().toString().c_str(),
      WiFi.subnetMask().toString().c_str(),
      WiFi.localIP().toString() != "(IP unset)" ? "<br/><br/><div><input type='hidden' id='upgradeMQTT' name='upgradeMQTT' value='false'/><br/>\
        <input class='factoryresetbutton' type='submit' onclick='submitFormUpgradeMQTT()' value='OTA Upgrade Gateway MQTT' />\
        <input type='text' id='upgradepathMQTT' name='upgradepathMQTT' size='40' value='http://shrdzm.pintarweb.net/upgrade.php'></div>\
        <br/><br />\
        <div><input type='hidden' id='upgradeESPNow' name='upgradeESPNow' value='false'/><br/>\
        <input class='factoryresetbutton' type='submit' onclick='submitFormUpgradeESPNow()' value='OTA Upgrade Gateway ESPNow' />\
        <input type='text' id='upgradepathESPNow' name='upgradepathESPNow' size='40' value='http://shrdzm.pintarweb.net/upgrade.php'></div>" : ""
  );  

  char * temp = getWebsite(menuContextBuffer, true);

  request->send_P(200, "text/html", (const uint8_t *)temp, strlen(temp));
}

void handleNotFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "Not found");
}

void handleReboot(AsyncWebServerRequest *request) 
{
  char temp[300];
  
  snprintf(temp, 300,
  "<!DOCTYPE html>\
  <html>\
  <head>\
  <meta http-equiv='refresh' content='10; url=/general'>\
  </head>\
  <body>\
  <h1>Please wait. Will reboot in 10 seconds...</h1>\
  </body>\
  </html>\
  ");

  shouldReboot = true;

  request->send_P(200, "text/html", (const uint8_t *)temp, strlen(temp));
}

void handleCloud(AsyncWebServerRequest *request)
{
  if(request->args() != 0)
  {
    if( request->hasArg("cloudEnabledChanged"))
    {
      DV(String(request->arg("cloudEnabledChanged")));
      if(String(request->arg("cloudEnabledChanged")) == "1") 
      {
        if( request->hasArg("cloudenabled"))
        {
          if(String(request->arg("cloudenabled")) == "on") 
          {
            configuration.setCloudParameter("enabled", "true");

            if(cloudRegisterNewUser(request->arg("user").c_str(), "", request->arg("password").c_str()))
            {
              cloudConnected = cloudLogin(request->arg("user").c_str(), request->arg("password").c_str());
              if(cloudConnected)
              {
                configuration.setCloudParameter("userid", cloudID.c_str());
              }
            }
          }
          else
          {
            configuration.setCloudParameter("enabled", "false");
          }
          DV(String(request->arg("cloudenabled")));
        }
        else
        {
          configuration.setCloudParameter("enabled", "false");
        }
      }      
    }
    if( request->hasArg("resetCloudSettings"))
    {
      DV(String(request->arg("resetCloudSettings")));
      if(String(request->arg("resetCloudSettings")) == "true") 
      {
        configuration.setCloudParameter("user", deviceName.c_str());
        configuration.setCloudParameter("password", String(ESP.getChipId()).c_str());
      }
    }
    
    writeConfiguration = true;          
  }  

  
  sprintf(menuContextBuffer,  
      handleCloud_template,
      String(configuration.getCloudParameter("enabled")) == "true" ? "checked" : "",
      configuration.getCloudParameter("user"),
      configuration.getCloudParameter("password"),
      (strlen(configuration.getCloudParameter("userid")) == 0) ? "<i><div style='color:#FF0000';>NOT REGISTERED</div></i>" : configuration.getCloudParameter("userid")
  );  

  char * temp = getWebsite(menuContextBuffer);

  DV(String(strlen(temp)));
  
  request->send_P(200, "text/html", (const uint8_t *)temp, strlen(temp));
}

void StoreMQTT(AsyncWebServerRequest *request)
{
  if(request->params())
  {
    DLN("MQTT Settings changed");

    if( request->hasArg("mqttEnabledChanged"))
    {
      if( request->hasArg("mqttenabled"))
      {
        if(String(request->arg("mqttenabled")) == "on") 
        {
          configuration.setWlanParameter("MQTTenabled", "true");
        }
        else
        {
          configuration.setWlanParameter("MQTTenabled", "false");
        }
        DV(String(request->arg("mqttenabled")));
      }
      else
      {
        configuration.setWlanParameter("MQTTenabled", "false");
      }
    }
    
    if(request->hasArg("MQTTbroker"))
      configuration.setWlanParameter("MQTTbroker", request->arg("MQTTbroker").c_str());
    if(request->hasArg("MQTTport"))
      configuration.setWlanParameter("MQTTport", request->arg("MQTTport").c_str());
    if(request->hasArg("MQTTuser"))
      configuration.setWlanParameter("MQTTuser", request->arg("MQTTuser").c_str());
    if(request->hasArg("MQTTpassword"))
      configuration.setWlanParameter("MQTTpassword", request->arg("MQTTpassword").c_str());

    //writeConfiguration = true;        
    configuration.store();

    if(mqttclient.connected())
      mqttclient.disconnect();
  }

  while(locked)
    delay(100);
    
  request->redirect("/general");
  DLN("nach webserver.redirect");
  
}

void handleFetchtest(AsyncWebServerRequest *request)
{
  String message = (F("{}")); // End of JSON

  Serial.println("handleFetchtest called");
 
  request->send(200, "application/json", message);
}

void handleWiFi(AsyncWebServerRequest *request)
{
  if(request->args() != 0)
  {
    if(request->hasArg("wlanform"))
    {
      DLN("WLAN Settings changed");
      if(request->hasArg("ssid"))
        configuration.setWlanParameter("ssid", request->arg("ssid").c_str());
      else
        configuration.setWlanParameter("ssid", "");
          
      if(request->hasArg("password"))
        configuration.setWlanParameter("password", request->arg("password").c_str());
      else
        configuration.setWlanParameter("password", "");          

      writeConfiguration = true;        
    }
    if(request->hasArg("mqttform"))
    {
      DLN("MQTT Settings changed");

      if(request->hasArg("MQTTbroker"))
        configuration.setWlanParameter("MQTTbroker", request->arg("MQTTbroker").c_str());
      if(request->hasArg("MQTTport"))
        configuration.setWlanParameter("MQTTport", request->arg("MQTTport").c_str());
      if(request->hasArg("MQTTuser"))
        configuration.setWlanParameter("MQTTuser", request->arg("MQTTuser").c_str());
      if(request->hasArg("MQTTpassword"))
        configuration.setWlanParameter("MQTTpassword", request->arg("MQTTpassword").c_str());

      writeConfiguration = true;        
    }
  }

  sprintf(menuContextBuffer,  
      handleWiFi_template,
      configuration.getWlanParameter("ssid"),
      configuration.getWlanParameter("password"),
      configuration.getWlanParameter("MQTTbroker"),
      configuration.getWlanParameter("MQTTport"),
      configuration.getWlanParameter("MQTTuser"),
      configuration.getWlanParameter("MQTTpassword")
  );  


  char * temp = getWebsite(menuContextBuffer);

  DV(String(strlen(temp)));
  
//  request->send(200, "text/html", temp); 
  request->send_P(200, "text/html", (const uint8_t *)temp, strlen(temp));

  DLN("nach webserver.send");
   
}


void handleNTP(AsyncWebServerRequest *request)
{
  if(request->args() != 0)
  {
    if(request->hasArg("ntpserver"))
      configuration.setWlanParameter("NTPServer", request->arg("ntpserver").c_str());

    if(request->hasArg("tz"))
      configuration.setWlanParameter("TZ", request->arg("tz").c_str());
        
    writeConfiguration = true;          
  }

  
  sprintf(menuContextBuffer,  
      "<h1>NTP</h1><p><strong></strong><br /><br />\
      <form method='post'>\
      <input type='text' id='ntpserver' name='ntpserver' placeholder='NTP Server' size='30' value='%s'>\
      <label for='ntpserver'>NTP Server</label><br/>\
      <br/>\
      <input type='text' id='tz' name='tz' placeholder='Time Zone' size='30' value='%s'>\
      <label for='tz'>Time Zone</label><br/>\
      <br/>\
      <br/><input class='submitbutton' type='submit' value='Save Configuration!' />\
      </form>\
      ",
      configuration.getWlanParameter("NTPServer"),
      configuration.getWlanParameter("TZ")
  );  

//  request->send(200, "text/html", getWebsite(menuContextBuffer));  
  char *temp = getWebsite(menuContextBuffer);

  request->send_P(200, "text/html", (const uint8_t *)temp, strlen(temp));
}

void startConfigurationAP()
{
  String APName = "SHRDZMGatewayMQTT-"+deviceName;
  WiFi.hostname(APName.c_str());        
  WiFi.softAP(APName);     

  DLN("Start configuration AP...");
  
  webserver.on("/", HTTP_ANY, handleRoot);
  webserver.on("/reboot", HTTP_ANY, handleReboot);
  webserver.on("/general", HTTP_ANY, handleRoot);
  webserver.on("/wifi", HTTP_ANY, handleWiFi);
  webserver.on("/cloud", HTTP_ANY, handleCloud);
  webserver.on("/NTP", HTTP_ANY, handleNTP);
  
  webserver.onNotFound(handleNotFound); 
  webserver.begin();

#ifdef LEDPIN
  pinMode(LEDPIN, OUTPUT);
  configurationBlinker.attach(0.2, changeConfigurationBlinker);  
#endif
}

void startGatewayWebserver()
{
  WiFi.mode(WIFI_STA);
  DLN("after WIFI_STA ");

  String APName = "SHRDZMGatewayMQTT-"+deviceName;
  
  WiFi.hostname(APName.c_str());
  WiFi.begin(configuration.getWlanParameter("ssid"), configuration.getWlanParameter("password"));
  DLN("after Wifi.begin");  

  apConnectingOngoing = true;                       
}

void startServerListener()
{
  webserver.on("/", HTTP_ANY, handleRoot);
  webserver.on("/reboot", HTTP_ANY, handleReboot);
  webserver.on("/general", HTTP_ANY, handleRoot);
  webserver.on("/wifi", HTTP_ANY, handleWiFi);
  webserver.on("/cloud", HTTP_ANY, handleCloud);
  webserver.on("/NTP", HTTP_ANY, handleNTP);  
  webserver.on("/j.js", HTTP_ANY, handleJs);      
  webserver.on("/json", HTTP_ANY, handleJson);  
  webserver.on("/fetchtest", HTTP_ANY, handleFetchtest);  
  webserver.onNotFound(handleNotFound); 
  webserver.begin();

  mqttclient.setServer(configuration.getWlanParameter("MQTTbroker"), 
                       atoi(configuration.getWlanParameter("MQTTport")));

  mqttclient.setCallback(mqttcallback);  
}  

void sendRCData(String data)
{
  DLN("Publish : topic : "+(String(MQTT_TOPIC)+"/RCData")+" Value: "+data);

  mqttclient.publish((String(MQTT_TOPIC)+"/RCData").c_str(), data.c_str());
}

bool mqttreconnect() 
{
  // Loop until we're reconnected
  if (!mqttclient.connected()) 
  {
//    DLN("Attempting MQTT connection...");
    String clientId = "SHRDZMGatewayMQTT-"+deviceName;
    // Attempt to connect
    mqttclient.setServer(configuration.getWlanParameter("MQTTbroker"), 
                     atoi(configuration.getWlanParameter("MQTTport")));

    if(mqttclient.connect(deviceName.c_str(), configuration.getWlanParameter("MQTTuser"), configuration.getWlanParameter("MQTTpassword")))
    {
      subscribeTopicSet = String(MQTT_TOPIC)+"/set";
      subscribeTopicConfig = String(MQTT_TOPIC)+"/config/set";
      
      DLN("connected");
      
      DLN("MQTTHost : "+String(configuration.getWlanParameter("MQTTbroker")));
      DLN("MQTTPort : "+String(configuration.getWlanParameter("MQTTport")));
      DLN("MQTTUser : "+String(configuration.getWlanParameter("MQTTuser")));
      DLN("MQTTPassword : xxxxxxxxxxxxxxxxxxx");
      DLN("MQTT_TOPIC : "+MQTT_TOPIC);
      DLN("MQTT_TOPIC_SUBSCRIBE Set : "+String(subscribeTopicSet));
      DLN("MQTT_TOPIC_SUBSCRIBE Config : "+String(subscribeTopicConfig));
      DLN("MQTT_TOPIC_SUBSCRIBE RCSEND : "+String(subscribeTopicRCSEND));
            
      // ... and resubscribe
      if(!mqttclient.subscribe(subscribeTopicSet.c_str()))
        DLN("Error at subscribe");
        
      mqttclient.subscribe(subscribeTopicConfig.c_str());
      mqttclient.subscribe(subscribeTopicRCSEND.c_str());
      
      // Once connected, publish an announcement...
      mqttclient.publish((String(MQTT_TOPIC)+"/state").c_str(), "up");
      mqttclient.publish((String(MQTT_TOPIC)+"/IP").c_str(), WiFi.localIP().toString().c_str()); 

#ifdef VERSION
      mqttclient.publish((String(MQTT_TOPIC)+"/version").c_str(), String(VERSION).c_str());
#else      
      mqttclient.publish((String(MQTT_TOPIC)+"/version").c_str(), "0.00");
#endif
      mqttclient.publish((String(MQTT_TOPIC)+"/gatewaymqttversion").c_str(), String(ver+"-"+currVersion).c_str());

      return true;
    } 
    else
    {
      DLN("mqtt not connected...");
      return false;
    }
  }
    
  return true;
}

void mqttcallback(char* topic, byte* payload, unsigned int length) 
{
  char* p = (char*)malloc(length+1);
  memcpy(p,payload,length);
  p[length] = '\0';
  String cmd = String(p);
  free(p);

  DLN("MQTT topic : "+String(topic));
  DLN("MQTT command : "+cmd);

  if(String(topic) == (String(MQTT_TOPIC)+"/RCSend"))
  {
    StringSplitter *splitter = new StringSplitter(cmd, ',', 3);
    int itemCount = splitter->getItemCount();
    
    if(itemCount == 3) // on/off housecode switchcode
    {
      bool onoff = false;
      String onoffText = splitter->getItemAtIndex(0);
      
      if(onoffText == "1"  || onoffText == "on" || onoffText == "ON" || onoffText == "ein" || onoffText == "EIN")
      {
        onoff = true;
      }

      if(onoff)
      {
        mySwitch.switchOn(splitter->getItemAtIndex(1).c_str(), splitter->getItemAtIndex(2).c_str());
      }
      else
      {
        mySwitch.switchOff(splitter->getItemAtIndex(1).c_str(), splitter->getItemAtIndex(2).c_str());
      }      
    }
    else
    {
      mySwitch.send(cmd.toInt(), 24);
      DLN("RCSend : "+cmd.toInt());
    }
  }
  
  if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd == "reset")
  {
    mqttclient.publish((String(MQTT_TOPIC)+"/state").c_str(), "reset");
    delay(1);

    ESP.reset();
  }
  else if(String(topic) == subscribeTopicConfig)
  {
      DLN("Config with parameter : "+cmd);
      // check if upgrade
      StringSplitter *splitter = new StringSplitter(cmd, ' ', 4);
      int itemCount = splitter->getItemCount();

      if(itemCount == 3 && splitter->getItemAtIndex(1) == "upgrade")
      {
        if(splitter->getItemAtIndex(0) == "GATEWAY")
        {
          String upgradeText = String("$upgrade "+WiFi.SSID()+"|"+WiFi.psk()+"|"+splitter->getItemAtIndex(2));
          
          DLN("Send upgrade of GATEWAY called : '"+upgradeText+"'");

          swSer.write(upgradeText.c_str());
          swSer.write('\n');           
        }
        else if(splitter->getItemAtIndex(0) == "GATEWAYMQTT") // upgrade yourself
        {
          DLN("Upgrade myself..");

          updateFirmware(splitter->getItemAtIndex(2));
        }
        else // Device upgrade called
        {
          String upgradeText = String("$set "+splitter->getItemAtIndex(0)+" upgrade "+WiFi.SSID()+"|"+WiFi.psk()+"|"+splitter->getItemAtIndex(2));
  
          DLN("Send upgrade : '"+upgradeText+"'");
          
          swSer.write(upgradeText.c_str());
          swSer.write('\n'); 
        }
      }
      else
      {
        swSer.write(String("$set "+cmd).c_str());
        swSer.write('\n'); 
      }
  }
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,9) == "getconfig")
  {
      swSer.write("$getconfig");
      swSer.write('\n');    
  }
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,13) == "configuration")
  {
      StringSplitter *splitter = new StringSplitter(cmd, ' ', 4);

      String configurationText = String("$configuration "+splitter->getItemAtIndex(0));
    
      swSer.write(configurationText.c_str());
      swSer.write('\n');    
  }
  else if(String(topic) == (String(MQTT_TOPIC)+"/set") && cmd.substring(0,5) == "pair ")
  {
      swSer.write("$pair");
      swSer.write('\n');    
  }
  
}
