void changeConfigurationBlinker()
{
#ifdef LEDPIN
  digitalWrite(LEDPIN, !(digitalRead(LEDPIN)));
#endif
}

char* getWebsite(char* content, bool update = false)
{  
  int len = strlen(content);

  memset(websideBuffer, 0, WEBSITEBUFFER_SIZE);
  DLN("Content len = "+String(len));

  sprintf(websideBuffer,  
"<!DOCTYPE html>\
<html>\
<head>\
<link rel='icon' type='image/svg+xml' href='https://shrdzm.pintarweb.net/Logo_min_red.svg' sizes='any'>\
%s\
<style>\
body {\
  font-family: Arial, Helvetica, sans-serif;\
}\
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
  width: 14em;\
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
cursor: pointer;\
}\
input.submitbutton, textarea {\
background: lightgray;\
border: 2px solid black;\
color: black;\
cursor: pointer;\
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
  background-color: Gray;\
  border: 2px solid black;\
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
  <li><a href='./cloud'>Cloud</a></li>\
  <li><a href='./NTP'>NTP</a></li>\
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
  ",
  update ? "<script src='j.js'></script>\n" : "", 
  deviceName.c_str(), deviceName.c_str(), content);

  return websideBuffer;
}

void handleJson() {
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
  webserver.send(200, "application/json", message);

  //DLN(message);
}

void handleJs()
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

  webserver.send(200, "text/javascript", message);
}

void handleRoot() 
{
  if(webserver.hasArg("factoryreset"))
  {
    if(String(webserver.arg("factoryreset")) == "true") // factory reset was pressed
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
        
      webserver.send(200, "text/html", menuContextBuffer);

      configuration.resetConfiguration();

      delay(2000);
      
      ESP.restart();        
    }
  }

  if(webserver.hasArg("upgradeMQTT") && webserver.hasArg("upgradepathMQTT"))
  {
    if(String(webserver.arg("upgradeMQTT")) == "true") // Upgrade was pressed
    {
      DLN("Upgrade from "+String(webserver.arg("upgradepathMQTT"))+" will be started...");
      //if(updateFirmwareByMQTT(server.arg("upgradepathMQTT")))
      if(updateFirmware(webserver.arg("upgradepathMQTT")))
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
          
        webserver.send(200, "text/html", menuContextBuffer);  
        return;      
      }
    }    
  }  

  if(webserver.hasArg("upgradeESPNow") && webserver.hasArg("upgradepathESPNow"))
  {
    if(String(webserver.arg("upgradeESPNow")) == "true") // Upgrade was pressed
    {
          String upgradeText = String("$upgrade "+WiFi.SSID()+"|"+WiFi.psk()+"|"+webserver.arg("upgradepathESPNow"));
          
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
            
          webserver.send(200, "text/html", menuContextBuffer);  
          return;      
    }    
  }  

  sprintf(menuContextBuffer,  
      "<h1>General</h1>\
      <img alt='SHRDZM' src='https://shrdzm.pintarweb.net/logo_200.png' width='200'>\
      <br /><br /><br /><br />\
Firmware Version : %s-%s<br>\
Chip ID : %s<br><br>\
MQTTTopic Set : %s<br>\
MQTTTopic Config : %s<br>\
MQTTTopic RCSEND : %s<br>\
MQTTTopic Sensordata : %s/<i><b>SensorID</b></i>/sensor/<br><br>\
MQTT Connection State :  <span id='mqttconnectionstate'>Unknown</span><br>\
Date/Time :  <span id='timestamp'>Unknown</span><br><br>\
Last Measurement : <br>\
<textarea readonly style='background-color:white;' id='lastmessage' name='lastmessage' cols='65' rows='10'></textarea><br><br>\
<br><br>IP : %s<br>\
DNS : %s<br>\
Gateway : %s<br>\
Subnet : %s<br>\
      <br/><br/>\
      <form method='post' id='factoryReset'>\
      <input type='hidden' id='factoryreset' name='factoryreset' value='false'/>\
      <input class='factoryresetbutton' type='submit' onclick='submitForm()' value='Factory Reset!' />\
      <br>\  
      %s\
      <script>\
       function submitForm()\
       {\
          document.getElementById('factoryreset').value = 'true';\
       }\
       function submitFormUpgradeMQTT()\
       {\
          document.getElementById('upgradeMQTT').value = 'true';\
       }\       
       function submitFormUpgradeESPNow()\
       {\
          document.getElementById('upgradeESPNow').value = 'true';\
       }\       
      </script>\
      </form>\
      ",
      ver.c_str(),
      String(ESP.getChipId()).c_str(),
      ESP.getSketchMD5().c_str(),
      subscribeTopicSet.c_str(),
      subscribeTopicConfig.c_str(),
      subscribeTopicRCSEND.c_str(),
      String(MQTT_TOPIC).c_str(), 
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

//  DLN("vor getWebsite");
  char * temp = getWebsite(menuContextBuffer, true);

//  Serial.printf("All Content len = %d\n",strlen(temp));

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

void handleCloud()
{

  
  if(webserver.args() != 0)
  {
    if( webserver.hasArg("cloudEnabledChanged"))
    {
      DV(String(webserver.arg("cloudEnabledChanged")));
      if(String(webserver.arg("cloudEnabledChanged")) == "1") 
      {
        if( webserver.hasArg("cloudenabled"))
        {
          if(String(webserver.arg("cloudenabled")) == "on") 
          {
            configuration.setCloudParameter("enabled", "true");

            if(cloudRegisterNewUser(webserver.arg("user").c_str(), "", webserver.arg("password").c_str()))
            {
              cloudConnected = cloudLogin(webserver.arg("user").c_str(), webserver.arg("password").c_str());
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
          DV(String(webserver.arg("cloudenabled")));
        }
        else
        {
          configuration.setCloudParameter("enabled", "false");
        }
      }      
    }
    if( webserver.hasArg("resetCloudSettings"))
    {
      DV(String(webserver.arg("resetCloudSettings")));
      if(String(webserver.arg("resetCloudSettings")) == "true") 
      {
        configuration.setCloudParameter("user", deviceName.c_str());
        configuration.setCloudParameter("password", String(ESP.getChipId()).c_str());
      }
    }
    
    writeConfiguration = true;          
  }  

  
  sprintf(menuContextBuffer,  
      "<h1>Cloud</h1><br/>\
      Cloud Settings are optional. More information can be found on <a href=\"http://shrdzm.com/\" target=\"_blank\">SHRDZM Homepage</a> and <a href=\"https://skills-store.amazon.de/deeplink/dp/B096S1675W?deviceType=app&share&refSuffix=ss_copy\" target=\"_blank\">Alexa</a>\
      <form name='cloudForm' method='post'>\
      <input type='hidden' id='resetCloudSettings' name='resetCloudSettings' value='false'/>\
      <br/>\
      <p>\
      <input type='checkbox' onClick='toggleCloudEnabled(this)' id='cloudenabled' name='cloudenabled' %s/>\
      <input type='hidden' id='cloudEnabledChanged' name='cloudEnabledChanged' value='0' />\
      <div><label for='cloudenabled'>Cloud Enabled</label></div><br/>\
      <br/>\
      <div><input type='text' id='user' name='user' placeholder='Name' size='30' value='%s'>\
      <label for='user'>User Name</label></div><br/><br/>\
      <div><input type='text' id='password' name='password' placeholder='Password' size='30' value='%s'>\
      <label for='user'>Password</label></div><br/><br/>\
      Unique User ID = %s<br/><br/>\
      <br /> <input type='submit' onclick='submitResetCloudSettings()' value='Reset Cloud Settings' /><br />\      
      <br /> <input class='submitbutton' type='submit' value='Save Cloud Settings!' />\      
      </p>\
      <script>\
      function submitResetCloudSettings()\
      {\
         document.getElementById('resetCloudSettings').value = 'true';\
      }\      
      function toggleCloudEnabled(f)\
      {\
        document.getElementById('cloudEnabledChanged').value = '1';\
        f.form.submit();\
      }\
      </script>\
      </form>\
      <br/>\
      <hr/>\
      ", 
      String(configuration.getCloudParameter("enabled")) == "true" ? "checked" : "",
      configuration.getCloudParameter("user"),
      configuration.getCloudParameter("password"),
      (strlen(configuration.getCloudParameter("userid")) == 0) ? "<i><div style='color:#FF0000';>NOT REGISTERED</div></i>" : configuration.getCloudParameter("userid")
  );  

  char * temp = getWebsite(menuContextBuffer);

  DV(String(strlen(temp)));
  
  webserver.send(200, "text/html", temp); 
}

void handleSettings()
{
  if(webserver.args() != 0)
  {
    if(webserver.hasArg("wlanform"))
    {
      DLN("WLAN Settings changed");
      if(webserver.hasArg("ssid"))
        configuration.setWlanParameter("ssid", webserver.arg("ssid").c_str());
      else
        configuration.setWlanParameter("ssid", "");
          
      if(webserver.hasArg("password"))
        configuration.setWlanParameter("password", webserver.arg("password").c_str());
      else
        configuration.setWlanParameter("password", "");          

      writeConfiguration = true;        
    }
    if(webserver.hasArg("mqttform"))
    {
      DLN("MQTT Settings changed");

      if(webserver.hasArg("MQTTbroker"))
        configuration.setWlanParameter("MQTTbroker", webserver.arg("MQTTbroker").c_str());
      if(webserver.hasArg("MQTTport"))
        configuration.setWlanParameter("MQTTport", webserver.arg("MQTTport").c_str());
      if(webserver.hasArg("MQTTuser"))
        configuration.setWlanParameter("MQTTuser", webserver.arg("MQTTuser").c_str());
      if(webserver.hasArg("MQTTpassword"))
        configuration.setWlanParameter("MQTTpassword", webserver.arg("MQTTpassword").c_str());

      writeConfiguration = true;        
    }
  }

  sprintf(menuContextBuffer,  
      "<h1>Settings</h1><br/>\
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
      <p><strong>WLAN</strong><br />\
      <form method='post'>\
      <input type='hidden' name='wlanform' value='1' />\
      <input type='text' id='ssid' name='ssid' placeholder='SSID' size='50' value='%s'>\
      <label for='ssid'>SSID</label><br/>\
      <br/>\
      <input type='password' id='password' name='password' placeholder='Password' size='50' value='%s'>\
      <label for='password'>Password</label><br/>\
      <br/>\
      <input type='checkbox' onclick='showWLANPassword()'>Show Password\
      <br/><br /> <input class='submitbutton' type='submit' value='Save WLAN Settings!' />\      
      </p>\      
      </form>\
      <br/>\
      <hr/>\
      <p><strong>MQTT</strong><br />\      
      <form method='post'>\
      <input type='hidden' name='mqttform' value='1' />\      
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
      <label for='MQTTuser'>MQTT Password</label><br/><br/><br/>\
      <br /> <input class='submitbutton' type='submit' value='Save MQTT Settings!' />\      
      </p>\      
      </form>\
      <br/>\
      <hr/>\
      ", 
      configuration.getWlanParameter("ssid"),
      configuration.getWlanParameter("password"),
      configuration.getWlanParameter("MQTTbroker"),
      configuration.getWlanParameter("MQTTport"),
      configuration.getWlanParameter("MQTTuser"),
      configuration.getWlanParameter("MQTTpassword")
  );  


  char * temp = getWebsite(menuContextBuffer);

  webserver.sendHeader("Content-Length", String(strlen(temp)));

  DV(String(strlen(temp)));
  
  webserver.send(200, "text/html", temp); 
  DLN("nach webserver.send");
   
}

void handleSettings_new()
{
  DLN("handleSettings");
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

    writeConfiguration = true;          
  }


  if((String(configuration.getCloudParameter("enabled")) == "true") &&
      cloudID != String(configuration.getCloudParameter("userid")))
  {
    configuration.setCloudParameter("userid", cloudID.c_str());
    
    writeConfiguration = true;          
  }
  
  // check if registerNewUser was pressed
  if(webserver.hasArg("registerNewUser"))
  {
    if(String(webserver.arg("registerNewUser")) == "true") 
    {
      if((webserver.hasArg("user") && strlen(webserver.arg("user").c_str()) > 3) &&
         (webserver.hasArg("cloudpassword") && strlen(webserver.arg("cloudpassword").c_str()) > 3))
      {
        if(cloudRegisterNewUser(webserver.arg("user").c_str(), webserver.arg("email").c_str(), webserver.arg("cloudpassword").c_str()))
        {
          if((String(configuration.getCloudParameter("enabled")) == "true"))
          {
            cloudConnected = cloudLogin(webserver.arg("user").c_str(), webserver.arg("cloudpassword").c_str());
            if(cloudConnected)
            {
              configuration.setCloudParameter("userid", cloudID.c_str());
              
              writeConfiguration = true;          
            }
          }        
        }
        writeConfiguration = true;          
      }
    }
  }  

  if(webserver.hasArg("unregisterUser"))
  {
    if(String(webserver.arg("unregisterUser")) == "true") 
    {
      if((webserver.hasArg("user") && strlen(webserver.arg("user").c_str()) > 3) &&
         (webserver.hasArg("cloudpassword") && strlen(webserver.arg("cloudpassword").c_str()) > 3))
      {
        if(cloudUnregisterUser(webserver.arg("user").c_str(), webserver.arg("cloudpassword").c_str()))
        {
          cloudConnected = false;
          configuration.setCloudParameter("userid", "");
          
          writeConfiguration = true;          
        }
      }
    }
  }  

  // check if registerNewUser was pressed
  if(webserver.hasArg("loginUser"))
  {
    if(String(webserver.arg("loginUser")) == "true") 
    {
      if((webserver.hasArg("user") && strlen(webserver.arg("user").c_str()) > 3) &&
         (webserver.hasArg("cloudpassword") && strlen(webserver.arg("cloudpassword").c_str()) > 3))
      {
        cloudConnected = cloudLogin(webserver.arg("user").c_str(), webserver.arg("cloudpassword").c_str());
        if(cloudConnected)
        {
          configuration.setCloudParameter("userid", cloudID.c_str());
          
          writeConfiguration = true;          
        }
      }
    }
  }  
  
  if(webserver.args() != 0)
  {
    if( webserver.arg("cloudenabled") == "1")
    {
      configuration.setCloudParameter("enabled", "true");
    }
    else
      configuration.setCloudParameter("enabled", "false");

    if(webserver.hasArg("user"))
      configuration.setCloudParameter("user", webserver.arg("user").c_str());
    else
      configuration.setCloudParameter("user", "");
        
    if(webserver.hasArg("cloudpassword"))
      configuration.setCloudParameter("password", webserver.arg("cloudpassword").c_str());
    else
      configuration.setCloudParameter("password", "");    

    if(webserver.hasArg("email"))
      configuration.setCloudParameter("email", webserver.arg("email").c_str());
    else
      configuration.setCloudParameter("email", "");    

    writeConfiguration = true;      
  }

  DLN("vor sprintf");

  sprintf(menuContextBuffer,  
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
      <label for='MQTTuser'>MQTT Password</label><br/><br/><br/>\
      <hr/>\
      Cloud Settings. More information can be found on <a href=\"http://shrdzm.com/\" target=\"_blank\">SHRDZM Homepage</a>\
      <br/><br/>\
      <input type='checkbox' id='cloudenabled' name='cloudenabled' value='1' %s/>\
      <input type='hidden' name='cloudenabled' value='0' />\
      <div><label for='cloudenabled'>Cloud Enabled</label></div><br/>\
      <br/><br/>\
      <div><input type='text' id='user' name='user' placeholder='Name' size='30' value='%s'>\
      <label for='user'>User Name</label></div><br/><br/>\
      Unique User ID = %s<br/><br/>\
      <div><input type='text' id='email' name='email' placeholder='name@example.com' size='30' value='%s'>\
      <label for='email'>EMail Address (optional)</label></div><br/>\
      <br/>\   
      <div><input type='password' id='cloudpassword' name='cloudpassword' placeholder='Password' size='30' value='%s'>\
      <label for='cloudpassword'>Password</label></div><br/><br/>\
      <div><input type='checkbox' onclick='showCloudPassword()'>Show Password\
      </div><br/>\
      %s\
      %s\  
      <hr/>\
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
      function showCloudPassword() {\
        var x = document.getElementById('cloudpassword');\
        if (x.type === 'password') {\
          x.type = 'text';\
        } else {\
          x.type = 'password';\
        }\
      }\      
      function submitLoginUser()\
      {\
         document.getElementById('loginUser').value = 'true';\
      }\      
      function submitRegisterNewUser()\
      {\
         document.getElementById('registerNewUser').value = 'true';\
      }\      
      function submitUnregisterUser()\
      {\
         document.getElementById('unregisterUser').value = 'true';\
      }\      
      </script>\ 
      </form>\
      ", 
      configuration.getWlanParameter("ssid"),
      configuration.getWlanParameter("password"),
      configuration.getWlanParameter("MQTTbroker"),
      configuration.getWlanParameter("MQTTport"),
      configuration.getWlanParameter("MQTTuser"),
      configuration.getWlanParameter("MQTTpassword"),
      String(configuration.getCloudParameter("enabled")) == "true" ? "checked" : "",
      configuration.getCloudParameter("user"),
      (strlen(configuration.getCloudParameter("userid")) == 0) ? "<i>NOT REGISTERED</i>" : configuration.getCloudParameter("userid"),
      configuration.getCloudParameter("email"),
      configuration.getCloudParameter("password"),      
      (strlen(configuration.getCloudParameter("userid")) == 0) ? "<div><p><input type='hidden' id='loginUser' name='loginUser' value='false'/>\
      <input class='submitbutton' type='submit' onclick='submitLoginUser()' value='LogIn User' /></p></div><br/><br/>\
      <div><p><input type='hidden' id='registerNewUser' name='registerNewUser' value='false'/>\
      <input class='submitbutton' type='submit' onclick='submitRegisterNewUser()' value='Register New User' /></p></div><br/><br/>" : "",
      (strlen(configuration.getCloudParameter("userid")) > 0) ? "<div><p><input type='hidden' id='unregisterUser' name='unregisterUser' value='false'/>\
      <input class='submitbutton' type='submit' onclick='submitUnregisterUser()' value='UnRegister User' /></p></div><br/><br/>" : "" 
  );  

  DLN("nach sprintf");

  char * temp = getWebsite(menuContextBuffer);
  DLN("nach getWebsite");

  webserver.sendHeader("Content-Length", String(strlen(temp)));

  DV(String(strlen(temp)));

  
  webserver.send(200, "text/html", temp); 
  DLN("nach webserver.send");
   
}

void handleNTP()
{
  if(webserver.args() != 0)
  {
    if(webserver.hasArg("ntpserver"))
      configuration.setWlanParameter("NTPServer", webserver.arg("ntpserver").c_str());

    if(webserver.hasArg("tz"))
      configuration.setWlanParameter("TZ", webserver.arg("tz").c_str());
        
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

  webserver.send(200, "text/html", getWebsite(menuContextBuffer));  
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
  webserver.on("/cloud", handleCloud);
  webserver.on("/NTP", handleNTP);
  
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
  webserver.on("/", handleRoot);
  webserver.on("/reboot", handleReboot);
  webserver.on("/general", handleRoot);
  webserver.on("/settings", handleSettings);
  webserver.on("/cloud", handleCloud);
  webserver.on("/NTP", handleNTP);  
  webserver.on("/j.js", handleJs);      
  webserver.on("/json", handleJson);  
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
