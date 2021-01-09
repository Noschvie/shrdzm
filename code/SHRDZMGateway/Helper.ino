char websideBuffer[5000];
String deviceName;
ESP8266WebServer webserver;
bool APModeEnabled = false;
bool storeConfiguration = false;

bool checkAP()
{
    pinMode(ACCESSPOINT_PIN, INPUT_PULLUP);  
    return digitalRead(ACCESSPOINT_PIN) ? false : true;
}

char* getWebsite(char* content)
{  
  int len = strlen(content);

  Serial.println("Content len = "+String(len));

  sprintf(websideBuffer,  
"<!DOCTYPE html>\
<html>\
<head>\
<link rel=\"icon\" type=\"image/svg+xml\" href=\"https://shrdzm.pintarweb.net/Logo_min.svg\" sizes=\"any\">\
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
  background-color: #9999ff;\
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
  width: 20em;\
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
<title>SHRDZMDevice - %s</title>\
</head>\
<body>\
\
<ul>\
  <li>\
    <a class='active' href='#home'>SHRDZMGateway<br/>\
      <font size='2'>%s</font>\
    </a></li>\
  <li><a href='./general'>General</a></li>\
  <li><a href='./settings'>Settings</a></li>\
  <li><a href='./about'>About</a></li>\
  <li><a href='./reboot'>Reboot</a></li>\
  <br/>\
  <li><font size='2' color='blue'><a href='http://shrdzm.com/' target='_blank'>\
  <img alt='SHRDZM' src='https://shrdzm.pintarweb.net/Logo.svg' width='60'>\
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

void handleSettings()
{
  char content[2600];

  if(webserver.args() != 0)
  {
    if(webserver.hasArg("ssid"))
      configuration.setWlanParameter("ssid", webserver.arg("ssid").c_str());
    else
      configuration.setWlanParameter("ssid", ""); 
        
    if(webserver.hasArg("ssidpassword"))
      configuration.setWlanParameter("password", webserver.arg("ssidpassword").c_str());     
    else
      configuration.setWlanParameter("password", ""); 
    
    if(webserver.hasArg("sim800"))
    {
      if( webserver.arg("sim800") == "1")
        configuration.setSim800Parameter("enabled", "true");
      else
        configuration.setSim800Parameter("enabled", "false");
    }
    else
    {
      configuration.setSim800Parameter("enabled", "false");
    }

    if(webserver.hasArg("pin"))
      configuration.setSim800Parameter("pin", webserver.arg("pin").c_str());    
    if(webserver.hasArg("apn"))
      configuration.setSim800Parameter("apn", webserver.arg("apn").c_str());    
    if(webserver.hasArg("user"))
      configuration.setSim800Parameter("user", webserver.arg("user").c_str());    
    if(webserver.hasArg("password"))
      configuration.setSim800Parameter("password", webserver.arg("password").c_str());    
    if(webserver.hasArg("MQTTbroker"))
      configuration.setSim800Parameter("MQTTbroker", webserver.arg("MQTTbroker").c_str());    
    if(webserver.hasArg("MQTTport"))
      configuration.setSim800Parameter("MQTTport", webserver.arg("MQTTport").c_str());    
    if(webserver.hasArg("MQTTuser"))
      configuration.setSim800Parameter("MQTTuser", webserver.arg("MQTTuser").c_str());    
    if(webserver.hasArg("MQTTpassword"))
      configuration.setSim800Parameter("MQTTpassword", webserver.arg("MQTTpassword").c_str());    

  }
      
  sprintf(content,   
      "<h1>Settings</h1><p><strong>Configuration</strong><br /><br />\
      <p>WLAN Settings only needed if OTA is used.</p>\
      <br/><br/>\
      <form method='post'>\
      <input type='text' id='ssid' name='ssid' placeholder='SSID' size='50' value='%s'>\
      <label for='ssid'>SSID</label><br/>\
      <br/>\
      <div><input type='password' id='ssidpassword' name='ssidpassword' placeholder='Password' size='50' value='%s'>\
      <label for='ssidpassword'>Password</label></div><br/><br/>\
      <div><input type='checkbox' onclick='showWLANPassword()'>Show Password\
      </div><br/>\
      <hr/>\
      </p>\
      <div><input type='checkbox' id='sim800' name='sim800' value='1' %s/>\
      <input type='hidden' name='sim800' value='0' />\
      <label for='sim800'>I have a SIM800 module attached</label></div><br/>\
      <br/>\
      <div><input type='number' id='pin' name='pin' placeholder='PIN' size='50' value='%s'>\
      <label for='pin'>PIN</label></div><br/>\
      <br/>\
      <input type='text' id='apn' name='apn' placeholder='APN' size='50' value='%s'>\
      <label for='apn'>APN</label><br/>\
      <br/>\
      <input type='text' id='user' name='user' placeholder='User' size='50' value='%s'>\
      <label for='user'>User</label><br/>\
      <br/>\
      <input type='text' id='passwort' name='password' placeholder='Password' size='50' value='%s'>\
      <label for='passwort'>Password</label><br/>\
      <br/>\
      <br/>\
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
        var x = document.getElementById('ssidpassword');\
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
      configuration.getSim800Parameter("enabled") == "true" ? "checked" : "",
      configuration.getSim800Parameter("pin"),
      configuration.getSim800Parameter("apn"),
      configuration.getSim800Parameter("user"),
      configuration.getSim800Parameter("password"),
      configuration.getSim800Parameter("MQTTbroker"),
      configuration.getSim800Parameter("MQTTport"),
      configuration.getSim800Parameter("MQTTuser"),
      configuration.getSim800Parameter("MQTTpassword"));  

  char * temp = getWebsite(content);
  webserver.send(200, "text/html", temp);  
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
void startConfigurationAP(String dn)
{
  deviceName = dn;
  String APName = "SHRDZM-GW-"+deviceName;

  WiFi.hostname(APName.c_str());        
  WiFi.softAP(APName);
  
  webserver.on("/", handleRoot);
  webserver.on("/general", handleRoot);
  webserver.on("/settings", handleSettings);
  webserver.on("/reboot", handleReboot);  
  webserver.begin();    
  APModeEnabled = true;
}

void helperLoop()
{
  if(APModeEnabled)
     webserver.handleClient();

  if(storeConfiguration)
  {
    storeConfiguration = false;
    configuration.store();
  }
}
