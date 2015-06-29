#include <Arduino.h>
#include "XBee.h"

XBee xbee = XBee();
uint8_t payload[] = {0,0};

XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
ZBTxRequest Tx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBRxResponse rx = ZBRxResponse();

void setup()
{
    Serial.begin(57600);
    xbee.setSerial(Serial);

    //TXTest();
}

void loop()
{
    RXTest();
    delay(10);
}

void TXTest()
{
    xbee.send(Tx);
}

void RXTest()
{
    xbee.readPacket();

    if (xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
    {
        xbee.getResponse().getZBRxResponse(rx);
        uint8_t* payload2 = rx.getData();
        //for(int i = 0; i <= sizeof(payload); i++)
        //payload[i] = rx.getData(i);

        //Tx.setPayloadLength(sizeof(payload2));
        //Tx.setPayload(payload2);
        Tx = ZBTxRequest(addr64, payload2, sizeof(payload2));
        xbee.send(Tx);
    }
}

