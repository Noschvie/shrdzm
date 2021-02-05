#ifndef SERIALBUFFEROBJECT_H
#define SERIALBUFFEROBJECT_H

class SerialBufferObject
{
  public:
    SerialBufferObject(){};
    ~SerialBufferObject(){};

  class SerialBufferItem
  {
    public:
      String m_text;

      SerialBufferItem(String text)
      {
        m_text = text;
      };      
  };

  void executeSerialSend()
  {
      SerialBufferItem *i = GetNextItem();
      if(i != NULL)
      {
        Serial.print(i->m_text.c_str());
        RemoveItem(i);
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
