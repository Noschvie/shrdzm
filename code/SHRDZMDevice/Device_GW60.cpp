#include "Device_GW60.h"

#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15


Device_GW60::Device_GW60()
{  
  deviceTypeName = "GW60";
  
  actionParameter = docAction.to<JsonObject>();  
  actionParameter["setpositionpercent"] = "10,20,30,40,50,60,70,80,90,100";
  actionParameter["initposition"] = "up,down";
  actionParameter["up"] = "noArg";
  actionParameter["down"] = "noArg";

  actionSet = false;
  m_laststate = "";
  remote = false;

  dir = 0;
  shadeposition = 0;
  count = 0;
  newpercentage = 0;
  counted = 0;
  pinD6 = 0;
  address = 0;

  SPIFFS.begin();
  readmaxcount();

  pinMode(D5, INPUT);         // Impulszähler
  pinMode(D3, INPUT_PULLUP);  // Motor fährt hoch
  pinMode(D4, INPUT_PULLUP);  // Motor fährt runter
  pinMode(D1, OUTPUT);        // Taste für runter
  digitalWrite(D1, HIGH);     // Setze Taste auf HIGH
  pinMode(D2, OUTPUT);        // Taste für hoch
  digitalWrite(D2, HIGH);     // Setze Taste auf HIGH
  pinMode(D6, INPUT_PULLUP);  // Test-Taste für Debug-Tests
}

Device_GW60::~Device_GW60()
{
  DLN(F("GW60 Instance deleted"));
}

bool Device_GW60::setDeviceParameter(JsonObject obj)
{
  DeviceBase::setDeviceParameter(obj);
}

bool Device_GW60::initialize()
{
  // create an object
  deviceParameter = doc.to<JsonObject>();

  return true;
}

boolean Device_GW60::isNumeric(String str) 
{ 
  unsigned int stringLength = str.length();
  
  if (stringLength == 0) {
      return false;
  }
  
  boolean seenDecimal = false;
  
  for(unsigned int i = 0; i < stringLength; ++i) 
  {
      if (isDigit(str.charAt(i))) 
      {
          continue;
      }
  
      if (str.charAt(i) == '.') 
      {
          if (seenDecimal) 
          {
              return false;
          }
          seenDecimal = true;
          continue;
      }
      return false;
  }
  return true;
}

void Device_GW60::savemaxcount()
{
  File configFile = SPIFFS.open("/GW60maxcount.txt", "w");
  if (!configFile) 
  {
    DLN(F("failed to open config file for writing"));
    return;
  }

  configFile.println(String(count));
  maxcount = count;
  configFile.close();
}

void Device_GW60::readmaxcount()
{
  File configFile = SPIFFS.open("/GW60maxcount.txt", "r");
  if (!configFile) 
  {
    DLN(F("failed to open config file for reading"));
    return;
  }

  maxcount = configFile.readStringUntil('\n').toInt();
//  maxcount = count;
  configFile.close();
}

bool Device_GW60::setAction(String action)
{
  String sCommand = getValue(action, ':', 0);
  
  if(sCommand == "setpositionpercent")
  {
    if(isNumeric(getValue(action, ':', 1).c_str()))
    {
      int tempnewpercentage = getValue(action, ':', 1).toInt();

      if ( ( tempnewpercentage > String(percentage).toInt() + 5 ) || ( tempnewpercentage < String(percentage).toInt() - 5 ) )
      {
        newpercentage = tempnewpercentage;
        remote = true;
        DLN(F("Neuen Prozentwert empfangen: "));
        DV(newpercentage);   
        actionSet = true;
      }
    }
  }
  else if(sCommand == "up")
  {
    up();
    actionSet = true;
  }
  else if(sCommand == "down")
  {
    down();
    actionSet = true;
  }
  else if(sCommand == "initposition")
  {
    if(getValue(action, ':', 1) == "up")
    {
      count = 0;      
    }
    else if(getValue(action, ':', 1) == "down")
    {
      savemaxcount();      
    }
  }
      
  return true;
}

void Device_GW60::up()
{
  digitalWrite(D2, LOW);

  lastActionStart = millis();
  
  m_laststate = "up";
  m_lastaction = "up";
}

void Device_GW60::down()
{
  digitalWrite(D1, LOW);

  lastActionStart = millis();

  m_laststate = "down";
  m_lastaction = "down";  
}

void Device_GW60::newposition()
{
  // -- wenn neuer Wert größer als aktueller ist und Rollladen aktuell gestoppt, dann starten
  if ( ( newpercentage > String(percentage).toInt() )  && dir == 0 && remote == true )
  {
    down(); // -- start
    DLN(F("Neuer Prozentwert unterscheidet sich, fahre runter"));
    
  // -- wenn aktueller Wert größer als neuer ist und momentan nach unten gefahren wird, dann stopp
  } else if ( ( String(percentage).toInt() > newpercentage )  && dir == 1 && remote == true )
  {
    up(); // -- stoppe jetzt
    Serial.println("Neuer Prozentwert ist gleich, stoppe jetzt");
    newpercentage = percentage;
    remote = false;
    Serial.println(remote);
    
  // -- wenn neuer Wert kleiner als aktueller ist und Rollladen aktuell gestoppt, dann starten
  } else if ( ( newpercentage < String(percentage).toInt() ) && dir == 0 && remote == true )
  {
    up(); // -- start
    Serial.println("Neuer Prozentwert unterscheidet sich, fahre hoch");
    
  // -- wenn aktueller Wert kleiner als neuer ist und momentan nach oben gefahren wird, dann stopp
  } else if ( ( String(percentage).toInt() < newpercentage )  && dir == -1 && remote == true )
  {
    down(); // -- stope jetzt
    Serial.println("Neuer Prozentwert ist gleich, stoppe jetzt");
    newpercentage = percentage;
    remote = false;
    Serial.println(remote);
  }
}

bool Device_GW60::loop()
{
  if(actionSet)
  {
      if(m_lastaction == "up" || m_lastaction == "down")
      {
        if(millis() > lastActionStart + 500 )
        {
          digitalWrite(D1, HIGH);
          digitalWrite(D2, HIGH);
          
          actionSet = false;
          m_lastaction = "";
        }
      }
  }

  countposition(); // -- Zähle aktuelle Position
  
  currentdirection(); // -- Aktuelle Richtung

  if ( (shadeposition != count)  && dir != 0 ){ // -- Aktuelle Position
    currentposition();
  }
    
  newposition(); //-- Neue Position
  
  return false;
}

SensorData* Device_GW60::readParameterTypes()
{
  SensorData *al = new SensorData(3);

  al->di[0].nameI = "positionpercent";
  al->di[1].nameI = "laststate";
  al->di[2].nameI = "moving";
  

  return al;
}

SensorData* Device_GW60::readInitialSetupParameter()
{
  SensorData *al = new SensorData(4);

  al->di[0].nameI = "interval";
  al->di[0].valueI = "0";
  al->di[1].nameI = "preparetime";
  al->di[1].valueI = "0";
  al->di[2].nameI = "processtime";
  al->di[2].valueI = "0";
  al->di[2].nameI = "sensorpowerpin";
  al->di[2].valueI = "99";

  return al;
}

bool Device_GW60::isNewDataAvailable()
{
  if(m_laststate != "")
    return true;

  if(m_moving != "")
    return true;

  return false;
}

SensorData* Device_GW60::readParameter()
{  
  if(m_laststate != "")
  {
    SensorData *al = new SensorData(1);
  
    al->di[0].nameI = "laststate";
    al->di[0].valueI = m_laststate;  

    m_laststate = "";
  
    return al;
  }
  else if(m_moving != "")
  {
    SensorData *al = new SensorData(1);
  
    al->di[0].nameI = "moving";
    al->di[0].valueI = m_moving;  

    m_moving = "";

    return al;
  }
  
  return NULL;
}

void Device_GW60::currentposition()
{
  char ccount[4];
  itoa( count, ccount, 10 );
//  mqttSend(mqttPositionTopic, ccount, mqttRetain);
  Serial.println("Aktuelle Position: " + String(count));
  // -- Prozentwerte berechen
  percentage = count / (maxcount * 0.01); 
  char cpercentage[5];
  itoa( percentage, cpercentage, 10 );
//  mqttSend(mqttPercentTopic, cpercentage, mqttRetain);
  Serial.println("Aktuelle Position in Prozent: " + String(percentage));
  
  shadeposition = count; // -- Speichern der aktuellen Position
}

void Device_GW60::currentdirection()
{
  if (digitalRead(D4) == LOW && dir != 1) 
  {
    m_moving = "Closing";
    dir = 1;
  } 
  else if (digitalRead(D3) == LOW && dir != -1) 
  {
    m_moving = "Opening";
    dir = -1;
  } 
  else if (digitalRead(D4) == HIGH && digitalRead(D3) == HIGH && dir != 0) 
  {
    m_moving = "Inactive";
    dir = 0;
    delay(1000);
  }
}

void Device_GW60::countposition() // -- Zählen der Positions-Impulse
{
  if (digitalRead(D5) == LOW && digitalRead(D3) == HIGH && dir == 1 && counted == 0) 
  {
    count++;
    Serial.println("Neuer Zähler: " + String(count));
    counted = 1; // -- sicherstellen, dass nur einmal pro LOW-Impuls gezählt wird
  }
  else if (digitalRead(D5) == LOW && digitalRead(D4) == HIGH && dir == -1 && counted == 0) 
  {
    if (count > 0)
    {
      count--;
    }
    Serial.println("Neuer Zähler: " + String(count));
    counted = 1; // -- sicherstellen, dass nur einmal pro LOW-Impuls gezählt wird
  } 
  else if (digitalRead(D5) == HIGH && counted == 1) 
  {
    counted = 0;
  }
}
String Device_GW60::getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++)
  {
    if(data.charAt(i)==separator || i==maxIndex)
    {
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
