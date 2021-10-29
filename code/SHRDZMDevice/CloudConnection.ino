#include "config/config.h"

#define DEBUG_SHRDZM

bool sendPrivateCloudData(const char* endpoint, const char* user, const char* password, const char* message )
{
  if(strncmp(endpoint, "https://", 8) != 0)
  {
    Serial.println(F("Only https connection allowed!"));
    return false;
  }

  String host(endpoint);
  host = host.substring(8);

  String path;
  int firstSlash = host.indexOf('/');
  int port = 443;

  if(firstSlash > 0)
  {
    path = host.substring(firstSlash);
    host = host.substring(0,firstSlash);

    if(host.indexOf(':') > 0)
    {
      port = atoi(host.substring(host.indexOf(':')+1).c_str());
      host = host.substring(0,host.indexOf(':'));
    }
  }  

  DLN(host);
  DLN(port);
  DLN(path);
  
    
  if(WiFi.status()== WL_CONNECTED)
  {  
    WiFiClientSecure client;
    client.setInsecure(); 
    client.setBufferSizes(512,512);
  
    HTTPClient http;
    if (!http.begin(client, host, port, path, true))
    {
      Serial.println(F("no connection possible!"));
    }      
  
    http.setAuthorization(user, password);
    http.addHeader("Content-Type", "application/json");
  
    int  httpCode = http.POST(message);

    Serial.println(httpCode);
    if(httpCode != 200)
    {
      
    }

  
    http.end();
  } 

  return true;
}

bool cloudRegisterNewUser(const char* user, const char* email, const char* password )
{
  DLN(F("Will now try to register new User on ")+String(CloudApiAddress));

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
  else if(doc["status"].as<unsigned int>() == 422)
    return true;
  else
    return false;
}

bool cloudUnregisterUser(const char* user, const char* password )
{
  DLN("Will now try to unregister User on "+String(CloudApiAddress));

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
  {
    DV(reply);
    return false;
  }
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
  DLN("Will now try to unregister Device on "+String(CloudApiAddress));

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
  String reply;

 // return cloudSendAsyncRESTCommand("measurement.php", String("{\"name\":\""+String(devicename)+"\",\"reading\":\""+String(reading)+"\",\"value\":\""+String(value)+"\"}").c_str(), true, &reply, true);
  //////// !!!!!!!!!!!!!!!
  
  return cloudSendRESTCommand("measurement.php", String("{\"name\":\""+String(devicename)+"\",\"reading\":\""+String(reading)+"\",\"value\":\""+String(value)+"\"}").c_str(), true, &reply, true);
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

////////// Async Handling //////////////////
bool cloudSendAsyncRESTCommand(const char* address, const char* content, bool tokenNeeded, String *reply, bool newTokenIfNeeded)
{
  if(WiFi.status()== WL_CONNECTED)
  {
    String finalAddress = String(CloudApiAddress) + String("/") + String(address);
//    request.setTimeout(1);

    request.open("POST", finalAddress.c_str());
    String auth = "Bearer " + cloudToken;    

    request.setReqHeader("Authorization", String(String("Basic ")+cloudToken).c_str());
    request.setReqHeader("Content-Type","application/json");
    request.send(content);
//    state = waitPost;    

    return true;
  }
    
/*    if(request.readyState() == 0 || request.readyState() == 4)
    {
        request.open("GET", "http://worldtimeapi.org/api/timezone/Europe/London.txt");
        request.send();
    }*/
}

void requestCB(void* optParm, asyncHTTPrequest* request, int readyState)
{
    if(readyState == 4)
    {
        Serial.println(request->responseText());
        Serial.println();
        request->setDebug(false);
    }
}
////////////////////////////////////////////

bool cloudSendRESTCommand(const char* address, const char* content, bool tokenNeeded, String *reply, bool newTokenIfNeeded)
{
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
  
    String finalAddress = String(CloudApiAddress) + String("/") + String(address);
//    DV(finalAddress);
//    DV(content);

    WiFiClient client;
        
//    if(http.begin(finalAddress))
    if(http.begin(client, finalAddress))
    {
      http.addHeader("Content-Type", "application/json");

      if(tokenNeeded && cloudToken != "")
      {
        http.addHeader(F("Authorization"), String(String("Basic ")+cloudToken).c_str());
      }      

      int httpResponseCode = http.POST(content);

      if(httpResponseCode != 200)
      {
        DV(httpResponseCode);
        DV(http.getString());
        return false;
      }
          
//      DV(httpResponseCode);
//      DV(http.getString());

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
