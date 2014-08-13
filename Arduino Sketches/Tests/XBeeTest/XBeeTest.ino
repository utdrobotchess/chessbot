#include <XBee.h>
#include <EEPROM.h>

XBee xbee = XBee();
uint8_t payload[] = {0,0,0,0};

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
ZBTxRequest Tx = ZBTxRequest(addr64, payload, sizeof(payload));

XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() 
{
  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() 
{   
  GetPayloadSizeTest();
}

void TXTest()
{
  xbee.send(Tx);
  delay(1000);
}

void RXTest()
{
  xbee.readPacket();
    
  if (xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) 
  {
    xbee.getResponse().getZBRxResponse(rx);
    for(int i = 0; i <= sizeof(payload); i++)
      payload[i] = rx.getData(i);
      
    Tx = ZBTxRequest(addr64, payload, sizeof(payload));
    xbee.send(Tx);
  } 
}

void GetPayloadSizeTest()
{
  xbee.readPacket();
    
  if (xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) 
  {
    xbee.getResponse().getZBRxResponse(rx);
    //payload[1] = rx.getDataLength();
      
    //Tx = ZBTxRequest(addr64, payload, sizeof(payload));
    xbee.send(Tx);
  } 
}

