#include <XBee.h>

XBee xbee = XBee();
uint8_t payload[] = {11,12};

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x403e0f30);
ZBTxRequest Tx = ZBTxRequest(addr64, payload, sizeof(payload));

void setup() 
{
  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() 
{   
}

void TXTest()
{
  xbee.send(Tx);
  delay(1000);
}

