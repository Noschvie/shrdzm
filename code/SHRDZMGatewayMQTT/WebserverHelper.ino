char* getWebsite(char* content)
{  
  int len = strlen(content);

  DLN("Content len = "+String(len));

  sprintf(websideBuffer,  
"<!DOCTYPE html>\
<html>\
<head>\
<link rel=\"icon\" type=\"image/svg+xml\" href=\"https://shrdzm.pintarweb.net/Logo_min_red.svg\" sizes=\"any\">\
<style>\
body {\
  font-family: Arial, Helvetica, sans-serif;\
}\
\
hr\
{ \
  display: block;\
  margin-top: 0.5em;\
  margin-bottom: 0.5em;\
  margin-left: auto;\
  margin-right: auto;\
  border-style: inset;\
  border-width: 1px;\
}\
ul \
{\
list-style-type: none;\
  margin: 0;\
  padding: 0;\
  width: 150px;\
  background-color: #f1f1f1;\
  position: fixed;\
  height: 100%;\
  overflow: auto;\
}\
\
li a {\
  display: block;\
  color: #000;\
  padding: 8px 16px;\
  text-decoration: none;\
}\
\
li a.active {\
  background-color: #ff0004;\
  color: white;\
}\
\
li a:hover:not(.active) {\
  background-color: #555;\
  color: white;\
}\
label.h2 {\
  font-weight: bold;\
  font-size: 150%;\
  width: 100%;\
  margin-bottom: 1em;\
}\
input,\
label {\
  float: left;\
  width: 40%;\
  margin-left: 1.5;\
  padding-left: 5px;\
}\
label {\
  display: inline-block;\
  width: 7em;\
}\
input {\
  margin: 0 0 1em .2em;\
  padding: .2em .5em;\
  background-color: #fffbf0;\
  border: 1px solid #e7c157;\
}\
label.input {\
  text-align: right;\
  margin-left: 10.5;\
  padding-left: 80px;\
  line-height: 1.5;\
}\
input.factoryresetbutton, textarea {\
background: cyan;\
border: 2px solid red;\
color: black;\
}\
button {\
  margin-top: 1.5em;\
  width: 30%;\
  border-radius: 10px;\
}\
.factoryresetbutton\
  background-color: Red;\
  border: 2px solid black;\
  border-radius: 5px;\
}\
.submitbutton {\
  background-color: Gainsboro;\
  border: 1px solid black;\
  border-radius: 5px;\
}\
.main {\
  margin-left: 200px;\
  margin-bottom: 30px;\
}\
</style>\
<title>SHRDZMGatewayMQTT - %s</title>\
</head>\
<body>\
\
<ul>\
  <li>\
    <a class='active' href='#home'><font size='1'>SHRDZMGatewayMQTT</font><br/>\
      <font size='2'>%s</font>\
    </a></li>\
  <li><a href='./general'>General</a></li>\
  <li><a href='./settings'>Settings</a></li>\
  <li><a href='./reboot'>Reboot</a></li>\
  <br/>\
  <li><font size='2' color='blue'><a href='http://shrdzm.com/' target='_blank'>\
  <img alt='SHRDZMGatewayMQTT' src='https://shrdzm.pintarweb.net/Logo.svg' width='60'>\
  Home</a></font></li>\
  <br/><br/><br/>\
  <li><center>&copy;&nbsp;<font size='2' color='darkgray'>Erich O. Pintar</font></center></li>\
  <br/><br/>\
</ul>\
\
<div class='main'>\
  %s\
</div>\
</body>\
</html>\
  ", deviceName.c_str(), deviceName.c_str(), content);

  return websideBuffer;
}

void handleRoot() 
{
  char content[2000];

  
  String informationTable = "<br><br>";  


  sprintf(content,  
      "<h1>General</h1>\
      <img alt='SHRDZM' src='https://shrdzm.pintarweb.net/logo_200.png' width='200'>\
      <br /><br /><br /><br />\
      %s\
      <br/><br/>\
      <form method='post' id='factoryReset'>\
      <input type='hidden' id='factoryreset' name='factoryreset' value='false'/>\
      <input class='factoryresetbutton' type='submit' onclick='submitForm()' value='Factory Reset!' />\
      <script>\
       function submitForm()\
       {\
          document.getElementById('factoryreset').value = 'true';\
       }\
      </script>\
      </form>\
      ",
      informationTable.c_str()
  );  

  char * temp = getWebsite(content);
  webserver.send(200, "text/html", temp);
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webserver.uri();
  message += "\nMethod: ";
  message += (webserver.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webserver.args();
  message += "\n";

  for (uint8_t i = 0; i < webserver.args(); i++) {
    message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "\n";
  }

  webserver.send(404, "text/plain", message);
}

void handleReboot() 
{
  char temp[300];
  
  snprintf(temp, 300,
  "<!DOCTYPE html>\
  <html>\
  <head>\
  <meta http-equiv='refresh' content='20; url=/'>\
  </head>\
  <body>\
  <h1>Please wait. Will reboot in 20 seconds...</h1>\
  </body>\
  </html>\
  ");


  webserver.send(200, "text/html", temp);

  delay(2000);
  
  ESP.restart();  
}

void handleSettings()
{
  char content[2200];

  if(webserver.args() != 0)
  {
    if(webserver.hasArg("ssid"))
      configuration.setWlanParameter("ssid", webserver.arg("ssid").c_str());
    else
      configuration.setWlanParameter("ssid", "");
        
    if(webserver.hasArg("password"))
      configuration.setWlanParameter("password", webserver.arg("password").c_str());
    else
      configuration.setWlanParameter("password", "");    
    if(webserver.hasArg("MQTTbroker"))
      configuration.setWlanParameter("MQTTbroker", webserver.arg("MQTTbroker").c_str());
    if(webserver.hasArg("MQTTport"))
      configuration.setWlanParameter("MQTTport", webserver.arg("MQTTport").c_str());
    if(webserver.hasArg("MQTTuser"))
      configuration.setWlanParameter("MQTTuser", webserver.arg("MQTTuser").c_str());
    if(webserver.hasArg("MQTTpassword"))
      configuration.setWlanParameter("MQTTpassword", webserver.arg("MQTTpassword").c_str());

    configuration.store();
    configuration.storeLastRebootInfo("normal");
  }


  sprintf(content,  
      "<h1>Settings</h1><p><strong>Configuration</strong><br /><br />\
      <form method='post'>\
      <input type='text' id='ssid' name='ssid' placeholder='SSID' size='50' value='%s'>\
      <label for='ssid'>SSID</label><br/>\
      <br/>\
      <input type='password' id='password' name='password' placeholder='Password' size='50' value='%s'>\
      <label for='password'>Password</label><br/>\
      <br/>\
      <input type='checkbox' onclick='showWLANPassword()'>Show Password\
      <br/>\
      <br/>\
      <hr/>\
      </p>\
      <input type='text' id='MQTTbroker' name='MQTTbroker' placeholder='MQTT Broker' size='50' value='%s'>\
      <label for='MQTTbroker'>MQTT Broker</label><br/>\
      <br/>\
      <input type='text' id='MQTTport' name='MQTTport' placeholder='MQTT Port' size='50' value='%s'>\
      <label for='MQTTbroker'>MQTT Port</label><br/>\
      <br/>\
      <input type='text' id='MQTTuser' name='MQTTuser' placeholder='MQTT User' size='50' value='%s'>\
      <label for='MQTTuser'>MQTT User</label><br/>\
      <br/>\
      <input type='text' id='MQTTpassword' name='MQTTpassword' placeholder='MQTT Password' size='50' value='%s'>\
      <label for='MQTTuser'>MQTT Password</label><br/>\
      <br/><br /> <input type='submit' value='Save Configuration!' />\
      <script>\
      function showWLANPassword() {\
        var x = document.getElementById('password');\
        if (x.type === 'password') {\
          x.type = 'text';\
        } else {\
          x.type = 'password';\
        }\
      }\
      </script>\ 
      </form>\
      ", 
      configuration.getWlanParameter("ssid"),
      configuration.getWlanParameter("password"),
      configuration.getWlanParameter("MQTTbroker"),
      configuration.getWlanParameter("MQTTport"),
      configuration.getWlanParameter("MQTTuser"),
      configuration.getWlanParameter("MQTTpassword") 
  );  

  char * temp = getWebsite(content);
  DLN("after getWebsite size = "+String(strlen(temp)));
  
  webserver.send(200, "text/html", temp);  
}

void startConfigurationAP()
{
  String APName = "SHRDZMGatewayMQTT-"+deviceName;
  WiFi.hostname(APName.c_str());        
  WiFi.softAP(APName);     

  DLN("Start configuration AP...");
  
  webserver.on("/", handleRoot);
  webserver.on("/reboot", handleReboot);
  webserver.on("/general", handleRoot);
  webserver.on("/settings", handleSettings);
  webserver.onNotFound(handleNotFound); 
  webserver.begin();
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
  webserver.on("/", handleRoot);
  webserver.on("/reboot", handleReboot);
  webserver.on("/general", handleRoot);
  webserver.on("/settings", handleSettings);
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
    DLN("Attempting MQTT connection...");
    String clientId = "SHRDZMGatewayMQTT-"+deviceName;
    // Attempt to connect
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
