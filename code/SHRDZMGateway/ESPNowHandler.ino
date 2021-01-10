
void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  DLN("MESSAGE:'"+String((char *)message)+"' from "+simpleEspConnection.macToStr(ad));
}

void OnSendError(uint8_t* ad)
{
  
}

void OnPaired(uint8_t *ga, String ad)
{
  DLN("EspNowConnection : Client '"+ad+"' paired! ");
}

void OnConnected(uint8_t *ga, String ad)
{
  
}

void OnPairingFinished()
{
  DLN("Pairing finished");
}
