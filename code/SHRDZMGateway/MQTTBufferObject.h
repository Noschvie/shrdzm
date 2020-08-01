#ifndef MQTTBUFFEROBJECT_H
#define MQTTBUFFEROBJECT_H

class MQTTBufferObject
{
  public:
    MQTTBufferObject(){};
    ~MQTTBufferObject(){};

  class BufferItem
  {
    public:
      String m_subject;
      String m_text;

      BufferItem(String subject, String text)
      {
        m_subject = subject;
        m_text = text;
      };      
  };

  void AddItem( String subject, String text )
  {
    for(int i = 0; i<50; i++)
    {
      if(items[i] == NULL)
      {
        items[i] = new BufferItem(subject, text);
        break;
      }
    }    
  };

  BufferItem* GetNextItem()
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

  void RemoveItem(BufferItem *it)
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
  
  BufferItem *items[50];  
};

#endif
