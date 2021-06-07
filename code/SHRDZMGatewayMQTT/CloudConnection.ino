#include "config/config.h"

void CloudLoop()
{
  if(registerDeviceTypeBuffer != "")
  {
    if(strcmp(configuration.getCloudParameter("enabled"),"true") == 0 && cloudConnected)
    {
      StringSplitter *splitter = new StringSplitter(registerDeviceTypeBuffer, ':', 2);
      if(splitter->getItemCount() == 2)
      {          
        cloudRegisterDevice(splitter->getItemAtIndex(0).c_str(), splitter->getItemAtIndex(1).c_str());
      }
    }
    
    registerDeviceTypeBuffer = "";
  }
}

bool cloudRegisterNewUser(const char* user, const char* email, const char* password )
{
  DLN("Will now try to register new User on "+String(CloudApiAddress));

  String reply;
  if(!cloudSendRESTCommand("register.php", String("{\"name\":\""+String(user)+"\",\"email\":\""+String(email)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply, false))
    return false;

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(doc["success"].as<unsigned int>() == 1)
    return true;
  else
    return false;
}

bool cloudUnregisterUser(const char* user, const char* password )
{
  DLN("Will now try to unregister new User on "+String(CloudApiAddress));

  String reply;
  if(!cloudSendRESTCommand("unregister.php", String("{\"name\":\""+String(user)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply, false))
    return false;

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(doc["success"].as<unsigned int>() == 1)
    return true;
  else
    return false;
}

bool cloudRegisterDevice(const char* devicename, const char* type )
{
  DLN("Will now try to register Device on "+String(CloudApiAddress));

  String reply;
  if(cloudSendRESTCommand("register-device.php", String("{\"name\":\""+String(devicename)+"\",\"type\":\""+String(type)+"\"}").c_str(), true, &reply, true))  
    return true;
  else
    return false;
}

bool cloudUnregisterDevice(const char* devicename)
{
  DLN("Will now try to register Device on "+String(CloudApiAddress));

  String reply;
  if(!cloudSendRESTCommand("unregister-device.php", String("{\"name\":\""+String(devicename)+"\"}").c_str(), true, &reply, true))
    return false;

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(doc["success"].as<unsigned int>() == 1)
    return true;
  else
    return false;
}

bool cloudIsDeviceRegisteredHere(const char* devicename)
{
  String reply;
  if(!cloudSendRESTCommand("device-info.php", String("{\"name\":\""+String(devicename)+"\"}").c_str(), true, &reply, true))
    return false;

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if((doc["success"].as<unsigned int>() == 1) && (doc["status"].as<unsigned int>() == 200))
  {
    if(doc["user"]["name"].as<String>() == String(configuration.getCloudParameter("user")))
      return true;
    else
      return false;
  }
  else
    return false;
}

bool cloudAddMeasurement(const char* devicename, const char* reading, const char* value )
{
  DLN("Will now try to add mesurement on "+String(CloudApiAddress));

  String reply;
  cloudSendRESTCommand("measurement.php", String("{\"name\":\""+String(devicename)+"\",\"reading\":\""+String(reading)+"\",\"value\":\""+String(value)+"\"}").c_str(), true, &reply, true);

  return true;
}

bool cloudLogin(const char* user, const char* password )
{
  String reply;
  if(!cloudSendRESTCommand("login.php", String("{\"name\":\""+String(user)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply, false))
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
  }
  else
    return false;
    
  return true;
}

bool cloudSendRESTCommand(const char* address, const char* content, bool tokenNeeded, String *reply, bool newTokenIfNeeded)
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

      if(tokenNeeded && cloudToken != "")
      {
        http.addHeader("Authorization", String(String("Basic ")+cloudToken).c_str());
      }      

      int httpResponseCode = http.POST(content);
          
      DV(httpResponseCode);
      DV(http.getString());

      *reply = http.getString();
      http.end();

      if(tokenNeeded && newTokenIfNeeded)
      {
        // check valid topken
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, *reply);
  
        if(doc["success"].as<unsigned int>() == 0)
        {
          if(doc["status"].as<unsigned int>() == 401) // Unauthorized
          {
            if(cloudLogin(configuration.getCloudParameter("user"), configuration.getCloudParameter("password")))
            {
              return cloudSendRESTCommand(address, content, tokenNeeded, reply, false);
            }
          }
        }
      }
      
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
