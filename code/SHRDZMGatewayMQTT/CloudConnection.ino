#include "config/config.h"

StaticJsonDocument<512> replyDoc;

void CloudLoop()
{
  if(registerDeviceTypeBuffer != "" && freeForRegistering)
  {
    if(strcmp(configuration.getCloudParameter("enabled"),"true") == 0 && cloudConnected)
    {
      StringSplitter *splitter = new StringSplitter(registerDeviceTypeBuffer, ':', 2);
      if(splitter->getItemCount() == 2)
      {     
        yield();     
        cloudRegisterDevice(splitter->getItemAtIndex(0).c_str(), splitter->getItemAtIndex(1).c_str());
        freeForRegistering = false;
      }
    }
    
    registerDeviceTypeBuffer = "";
  }
}

bool cloudRegisterNewUser(const char* user, const char* email, const char* password )
{
  DLN("Will now try to register new User on "+String(CloudApiAddress));

  String reply;
  if(!cloudSendRESTCommand("register.php", String("{\"name\":\""+String(user)+"\",\"email\":\""+String(email)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply))
    return false;

  DeserializationError error = deserializeJson(replyDoc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(replyDoc["success"].as<unsigned int>() == 1)
    return true;
  else
    return false;
}

bool cloudUnregisterUser(const char* user, const char* password )
{
  DLN("Will now try to unregister new User on "+String(CloudApiAddress));

  String reply;
  if(!cloudSendRESTCommand("unregister.php", String("{\"name\":\""+String(user)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply))
    return false;

  DeserializationError error = deserializeJson(replyDoc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(replyDoc["success"].as<unsigned int>() == 1)
    return true;
  else
    return false;
}

bool cloudRegisterDevice(const char* devicename, const char* type )
{
  DLN("Will now try to register Device on "+String(CloudApiAddress));

  String reply;
  if(cloudSendRESTCommand("register-device.php", String("{\"name\":\""+String(devicename)+"\",\"type\":\""+String(type)+"\"}").c_str(), true, &reply))  
    return true;
  else
    return false;
}

bool cloudUnregisterDevice(const char* devicename)
{
  DLN("Will now try to register Device on "+String(CloudApiAddress));

  String reply;
  if(!cloudSendRESTCommand("unregister-device.php", String("{\"name\":\""+String(devicename)+"\"}").c_str(), true, &reply))
    return false;

  DeserializationError error = deserializeJson(replyDoc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(replyDoc["success"].as<unsigned int>() == 1)
    return true;
  else
    return false;
}

bool cloudIsDeviceRegisteredHere(const char* devicename)
{
  String reply;
  if(!cloudSendRESTCommand("device-info.php", String("{\"name\":\""+String(devicename)+"\"}").c_str(), true, &reply))
    return false;

  DeserializationError error = deserializeJson(replyDoc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if((replyDoc["success"].as<unsigned int>() == 1) && (replyDoc["status"].as<unsigned int>() == 200))
  {
    if(replyDoc["user"]["name"].as<String>() == String(configuration.getCloudParameter("user")))
      return true;
    else
      return false;
  }
  else
    return false;
}

bool cloudAddMeasurement(const char* devicename, const char* reading, const char* value )
{
  String reply;
  cloudSendRESTCommand("measurement.php", String("{\"name\":\""+String(devicename)+"\",\"reading\":\""+String(reading)+"\",\"value\":\""+String(value)+"\"}").c_str(), true, &reply);

  return true;
}

bool cloudLogin(const char* user, const char* password )
{
  String reply;
  if(!cloudSendRESTCommand("login.php", String("{\"name\":\""+String(user)+"\",\"password\":\""+String(password)+"\"}").c_str(), false, &reply))
    return false;

  DeserializationError error = deserializeJson(replyDoc, reply);

  if(error)
  {
    DV(error.c_str());
    return false;  
  }

  if(replyDoc["success"].as<unsigned int>() == 1)
  {
    sprintf(cloudTokenArray, "Basic %s", replyDoc["token"].as<const char*>());
    cloudID = replyDoc["id"].as<String>();
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
//    DV(finalAddress);
//    DV(content);
        
    if(http.begin(finalAddress))
    {
      http.addHeader("Content-Type", "application/json");

      if(tokenNeeded && strlen(cloudTokenArray) > 0)
      {
        http.addHeader("Authorization", cloudTokenArray);
      }      

      int httpResponseCode = http.POST(content);

      if(httpResponseCode != 200) // Transaction OK
      {
        DV(httpResponseCode);
        return false;
      }
          
//      DV(httpResponseCode);
//      DV(http.getString());

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
