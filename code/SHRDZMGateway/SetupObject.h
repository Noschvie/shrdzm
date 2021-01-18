class SetupObject
{
  public:
  SetupObject()
  {
    m_simpleEspConnection = NULL;
  }
  
  class SetupItem
  {
    public:
    String m_deviceName;
    String m_parameterName;
    String m_parameterValue;
    unsigned long timestamp;

    public:
    SetupItem(String deviceName, String parameterName, String parameterValue)
    {
      m_deviceName = deviceName;
      m_parameterName = parameterName;
      m_parameterValue = parameterValue;
      timestamp = millis();
    };
    SetupItem(String deviceName, String command)
    {
      m_deviceName = deviceName;
      m_parameterName = command;
      m_parameterValue = "";
      timestamp = millis();
    };
  };

  protected:
  SimpleEspNowConnection *m_simpleEspConnection;

  public:
  SetupItem *items[20];

  void init(SimpleEspNowConnection *simpleEspConnection)
  {
    m_simpleEspConnection = simpleEspConnection;
  }
  
  void AddItem( String deviceName, String parameterName, String parameterValue )
  {
    bool bFoundSpace = false;
    SetupItem *oldestItem = NULL;
    
    for(int i = 0; i<20; i++)
    {
      if(items[i] == NULL)
      {
        items[i] = new SetupItem(deviceName, parameterName, parameterValue);
        bFoundSpace = true;
        break;
      }
      else
      {
        if(oldestItem == NULL)
          oldestItem = items[i];
          
        if(items[i]->timestamp < oldestItem->timestamp)
          oldestItem = items[i];
      }
    }

    if(!bFoundSpace && oldestItem != NULL) // search for the oldest one and take that
    {
#ifdef DEBUG
      Serial.printf("Oldest Item %s for %s removed.\n", oldestItem->m_parameterName.c_str(), oldestItem->m_deviceName.c_str());
#endif      
      RemoveItem(oldestItem);
      AddItem(deviceName, parameterName, parameterValue);
    }
  };

  void AddItem( String deviceName, String command )
  {
    bool bFoundSpace = false;
    SetupItem *oldestItem = NULL;
    
    for(int i = 0; i<20; i++)
    {
      if(items[i] == NULL)
      {
        items[i] = new SetupItem(deviceName, command);
        bFoundSpace = true;
        break;
      }
      else
      {
        if(oldestItem == NULL)
          oldestItem = items[i];
          
        if(items[i]->timestamp < oldestItem->timestamp)
          oldestItem = items[i];
      }
    }
    
    if(!bFoundSpace && oldestItem != NULL) // search for the oldest one and take that
    {
#ifdef DEBUG
      Serial.printf("Oldest Item %s for %s removed.\n", oldestItem->m_parameterName.c_str(), oldestItem->m_deviceName.c_str());
#endif
      RemoveItem(oldestItem);
      AddItem(deviceName, command);
    }
  };
  
  void AddInitItem( String deviceName )
  {
    for(int i = 0; i<20; i++)
    {
      if(items[i] == NULL)
      {
        items[i] = new SetupItem(deviceName, "init", "");
        break;
      }
    }
  };
  
  SetupItem* GetItem( String deviceName )
  {
    SetupItem *it = NULL;
    
    for(int i = 0;i<20; i++)
    {
      if(items[i] != NULL)
      {
        if(items[i]->m_deviceName == deviceName)
        {
          it = items[i];
        }
        break;
      }
    }

    return it;
  };  

  void RemoveItem(SetupItem *it)
  {
    for(int i = 0;i<20; i++)
    {
      if(items[i] != NULL && items[i] == it)
      {
        delete items[i];
        items[i] = NULL;
        break;
      }
    }
  };
};
