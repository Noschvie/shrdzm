#include "config/config.h"


bool cloudRegisterNewUser(const char* user, const char* email, const char* password )
{
  DLN("Will now try to register new User on "+String(CloudApiAddress));

  String reply;
  cloudSendRESTCommand("register.php", String("{\"name\":\""+String(user)+"\",\"email\":\""+String(email)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply);

  return true;
}

bool cloudLogin(const char* user, const char* password )
{
  DLN("Will now try Log On to "+String(CloudApiAddress));

  String reply;
  if(!cloudSendRESTCommand("login.php", String("{\"name\":\""+String(user)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply))
    return false;

  DV(reply);

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(doc["success"].as<unsigned int>() == 1)
  {
    cloudToken = doc["token"].as<String>();
    cloudID = doc["id"].as<String>();

    DV(cloudID);
  }
  else
    return false;
    
  return true;
}

bool cloudSendRESTCommand(const char* address, const char* content, bool tokenNeeded, String *reply)
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

      *reply = http.getString();
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
