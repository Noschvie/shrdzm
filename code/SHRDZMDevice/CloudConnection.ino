#include "config/config.h"

String cloudToken = "";

bool cloudRegisterNewUser(const char* user, const char* email, const char* password )
{
  DLN("Will now try to register new User on "+String(CloudApiAddress));

  cloudSendRESTCommand("register.php", String("{\"name\":\""+String(user)+"\",\"email\":\""+String(email)+"\",\"password\":\""+String(password)+"\"}").c_str(), false);

  return true;
}

bool cloudLogin(const char* user, const char* password )
{
  DLN("Will now try to register new User on "+String(CloudApiAddress));

  cloudSendRESTCommand("login.php", String("{\"name\":\""+String(user)+"\",\"password\":\""+String(password)+"\"}").c_str(), false);

  return true;
}

bool cloudSendRESTCommand(const char* address, const char* content, bool tokenNeeded)
{
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
  
    String finalAddress = String(CloudApiAddress) + String("/") + String(address);
    DV(finalAddress);
    DV(content);
        
    if(http.begin(finalAddress))
    {
      http.addHeader("Content-Type", "application/json");
    
      int httpResponseCode = http.POST(content);
          
      DV(httpResponseCode);
      DV(http.getString());
        
      http.end();
      return true;
    }
    else
    {
      DLN("http.begin not passed");
      return false;
    }
  }
  else
  {
    DLN("WiFi not connected");
    return false;
  }
}
