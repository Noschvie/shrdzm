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
  <li><a href='./mqtt'>MQTT</a></li>\
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
