#ifndef SERIALBUFFEROBJECT_H
#define SERIALBUFFEROBJECT_H

class SerialBufferObject
{
  public:
    SerialBufferObject(){sum1 = 0; sum2 = 0;};
    ~SerialBufferObject(){};
    uint8_t sum1;
    uint8_t sum2;

  class SerialBufferItem
  {
    public:
      String m_text;

      SerialBufferItem(String text)
      {
        m_text = text;
      };      
  };

  void calculateChecksum(String& message)
  {
    sum1 = 0;
    sum2 = 0;
        
    for(int i = 4; i<message.length(); i++)
    {
      sum1 = (sum1 + message[i]) % 255;
      sum2 = (sum2 + sum1) % 255;
    }

    message[2] = sum1;
    message[3] = sum2;
  }

  void executeSerialSend()
  {
      SerialBufferItem *i = GetNextItem();
      if(i != NULL)
      {
        i->m_text[1] = 'A';
        calculateChecksum(i->m_text);
        Serial.print(i->m_text.c_str());
        Serial.print('\n');
        RemoveItem(i);
        delay(200);
        yield();
      }    
  }

  void AddItem(String text)
  {
    for(int i = 0; i<50; i++)
    {
      if(items[i] == NULL)
      {
        items[i] = new SerialBufferItem(text);
        break;
      }
    }    
  };

  SerialBufferItem* GetNextItem()
  {
    for(int i = 0;i<50; i++)
    {
      if(items[i] != NULL)
      {
        return items[i];
        break;
      }
    }

    return NULL;
  };  

  void RemoveItem(SerialBufferItem *it)
  {
    for(int i = 0;i<50; i++)
    {
      if(items[i] != NULL && items[i] == it)
      {
        delete items[i];
        items[i] = NULL;
        break;
      }
    }
  };
  
  SerialBufferItem *items[50];  
    
};

#endif
