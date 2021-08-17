const static char website_template[] PROGMEM = 
{
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
  <li><a href='./wifi'>WiFi</a></li>\
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
</html>"
};

const static char handleRoot_template[] PROGMEM = 
{
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
Cloud Update :  %s<br><br>\
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
</form>"
};

const static char handleCloud_template[] PROGMEM = 
{
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
<hr/>"
};

const static char handleWiFi_template[] PROGMEM = 
{
"<h1>WiFi</h1><br/>\
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
<hr/>"
}; 
