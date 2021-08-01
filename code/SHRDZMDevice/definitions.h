// <select name='devices' onchange='this.form.submit()'>\


const static char settings_template[] PROGMEM = 
{      
"<h1>Settings</h1><p><strong>Configuration</strong><br /><br />\
<form method='post' id='settingsForm'>\
<label>Device Type :\
<select name='devices' onchange='submitDeviceChange(this)'>\
%s\
</select>\
</label>\
<br/><br/>\
%s\
<br/><br/>\
<input type='hidden' id='save' name='save' value='false'/>\
<input type='hidden' id='devicechanged' name='devicechanged' value='false'/>\
<input class='submitbutton' type='submit' onclick='submitForm()' value='Save Configuration!' />\
<script>\
function submitForm()\
{\
document.getElementById('save').value = 'true';\
}\
function submitDeviceChange(f)\
{\
document.getElementById('devicechanged').value = 'true';\
f.form.submit();\
}\
</script>\
</form>"
};

const static char upgrade_template[] PROGMEM = 
{        
"<!DOCTYPE html>\
<html>\
<head>\
</head>\
<body>\
<h1>Upgrade started. SHRDZMDevice will reboot after upgrade.</h1>\
</body>\
</html>"
};

const static char upgradetext_template[] PROGMEM = 
{
"<br/><br/><input type='hidden' id='upgrade' name='upgrade' value='false'/><br/>\
<input class='factoryresetbutton' type='submit' onclick='submitFormUpgrade()' value='OTA Upgrade' />\
<input type='text' id='upgradepath' name='upgradepath' size='35' value='http://shrdzm.pintarweb.net/upgrade.php'>\
<br/>"  
};

const static char handleReboot_template[] PROGMEM = 
{
"<!DOCTYPE html>\
<html>\
<head>\
<meta http-equiv='refresh' content='20; url=/'>\
</head>\
<body>\
<h1>Please wait. Will reboot in 20 seconds...</h1>\
</body>\
</html>"
};

const static char handleGateway_template[] PROGMEM = 
{
"<h1>Gateway</h1><p><strong>Configuration</strong><br /><br />\
WLAN Settings if Device acts as it's own gateway.\
<br/><br/>\
<form method='post'>\
<input type='checkbox' id='wlanenabled' name='wlanenabled' value='1' %s/>\
<input type='hidden' name='wlanenabled' value='0' />\
<div><label for='wlanenabled'>Device should act as it's own gateway</label></div><br/>\
<br/><br/>\
<hr/>\
<div><input type='text' id='ssid' name='ssid' placeholder='SSID' size='50' value='%s'>\
<p><label for='ssid'>SSID</label></p></div><br/>\
<br/>\
<div><input type='password' id='password' name='password' placeholder='Password' size='50' value='%s'>\
<label for='password'>Password</label></div><br/><br/>\
<div><input type='checkbox' onclick='showWLANPassword()'>Show Password\
</div><br/>\
<div><input type='text' id='ip' name='ip' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
<label for='ip'>Static IP</label></div><br/>\
<br/>\
<div><input type='text' id='dns' name='dns' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
<label for='ip'>DNS</label></div><br/>\
<br/>\
<div><input type='text' id='gateway' name='gateway' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
<label for='gateway'>Gateway</label></div><br/>\
<br/>\
<div><input type='text' id='subnet' name='subnet' placeholder='XXX.XXX.XXX.XXX' size='50' value='%s'>\
<label for='gateway'>Subnet</label></div><br/>\
<br/><br/>\
<div><input type='text' id='MQTTbroker' name='MQTTbroker' placeholder='MQTT Broker' size='50' value='%s'>\
<label for='MQTTbroker'>MQTT Broker</label></div><br/>\
<br/>\
<div><input type='text' id='MQTTport' name='MQTTport' placeholder='MQTT Port' size='50' value='%s'>\
<label for='MQTTport'>MQTT Port</label></div><br/>\
<br/>\
<div><input type='text' id='MQTTuser' name='MQTTuser' placeholder='MQTT User' size='50' value='%s'>\
<label for='MQTTuser'>MQTT User</label></div><br/>\
<br/>\
<div><input type='text' id='MQTTpassword' name='MQTTpassword' placeholder='MQTT Password' size='50' value='%s'>\
<label for='MQTTpassword'>MQTT Password</label></div><br/>\
<br/><br />\
<input class='submitbutton' type='submit' value='Save Configuration!' />\
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
</form>"  
};

const static char handleNTP_template[] PROGMEM = 
{
"<h1>NTP</h1><p><strong>Configuration</strong><br /><br />\
<form method='post' id='settingsForm'>\
<br/><br/><div><label for='ntpserver'>NTP Server</label>\
<input type='text' id='ntpserver' name='ntpserver' size='20' value='at.pool.ntp.org'></div>\
<br/><br/>\
<input type='hidden' id='save' name='save' value='false'/>\
<input class='submitbutton' type='submit' onclick='submitForm()' value='Save Configuration!' />\
<script>\
function submitForm()\
{\
document.getElementById('save').value = 'true';\
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

const static char handleRoot_template[] PROGMEM = 
{
"<h1>General</h1>\
<img alt='SHRDZM' src='https://shrdzm.pintarweb.net/logo_200.png' width='200'>\
<br /><br /><br /><br />\
%s\
<br/><br/>\
<form method='post' id='factoryReset'>\
<input type='hidden' id='factoryreset' name='factoryreset' value='false'/>\
<input class='factoryresetbutton' type='submit' onclick='submitFormFactoryReset()' value='Factory Reset!' />\
%s\
<script>\
function submitFormFactoryReset()\
{\
document.getElementById('factoryreset').value = 'true';\
}\
function submitFormUpgrade()\
{\
document.getElementById('upgrade').value = 'true';\
}\
</script>\
</form>"  
};

const static char factoryreset_template[] PROGMEM = 
{
"<br/><br/><input type='hidden' id='upgrade' name='upgrade' value='false'/><br/>\
<input class='factoryresetbutton' type='submit' onclick='submitFormUpgrade()' value='OTA Upgrade' />\
<input type='text' id='upgradepath' name='upgradepath' size='35' value='http://shrdzm.pintarweb.net/upgrade.php'>\
<br/>"  
};

const static char website_template[] PROGMEM = 
{
"<!DOCTYPE html>\
<html>\
<head>\
<link rel=\"icon\" type=\"image/svg+xml\" href=\"https://shrdzm.pintarweb.net/Logo_min_green.svg\" sizes=\"any\">\
%s\
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
background-color: #4CAF50;\
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
width: 80%;\
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
}\
input.submitbutton, textarea {\
background: lightgray;\
border: 2px solid black;\
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
background-color: Gray;\
border: 2px solid black;\
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
<a class='active' href='#home'>SHRDZMDevice<br/>\
<font size='2'>%s</font>\
</a></li>\
<li><a href='./general'>General</a></li>\
<li><a href='./settings'>Settings</a></li>\
<li><a href='./gateway'>Gateway</a></li>\
<li><a href='./NTP'>NTP</a></li>\
<li><a href='./cloud'>Cloud</a></li>\
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
</html>"
};
