

void RCSwitchLoop()
{
#ifdef RCSWITCHPIN
#ifdef RCSENDPIN
  
  if (mySwitch.available()) 
  {
    if(millis() - lastRCMillis > 1000)
    {
      unsigned long RCData = mySwitch.getReceivedValue();
      sendRCData(String(RCData));

      DLN("Incoming RCData : "+String(RCData));
    }

    lastRCMillis = millis();
    mySwitch.resetAvailable();
  }
  
#endif
#endif  
}
