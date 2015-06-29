#include "XBeeResponse.h"

/*-----------------------------------------------------------------------------------------
  XBeeResponse Function Members
  -----------------------------------------------------------------------------------------*/

void XBeeResponse::getZBTxStatusResponse(XBeeResponse &zbXBeeResponse) {

    // way off?
    ZBTxStatusResponse* zb = static_cast<ZBTxStatusResponse*>(&zbXBeeResponse);
    // pass pointer array to subclass
    zb->setFrameData(getFrameData());
    setCommon(zbXBeeResponse);
}

void XBeeResponse::getZBRxResponse(XBeeResponse &rxResponse) {

    ZBRxResponse* zb = static_cast<ZBRxResponse*>(&rxResponse);

    //TODO verify response api id matches this api for this response

    // pass pointer array to subclass
    zb->setFrameData(getFrameData());
    setCommon(rxResponse);

    zb->getRemoteAddress64().setMsb((uint32_t(getFrameData()[0]) << 24) + (uint32_t(getFrameData()[1]) << 16) + (uint16_t(getFrameData()[2]) << 8) + getFrameData()[3]);
    zb->getRemoteAddress64().setLsb((uint32_t(getFrameData()[4]) << 24) + (uint32_t(getFrameData()[5]) << 16) + (uint16_t(getFrameData()[6]) << 8) + (getFrameData()[7]));
}

void XBeeResponse::getModemStatusResponse(XBeeResponse &modemStatusResponse) {

    ModemStatusResponse* modem = static_cast<ModemStatusResponse*>(&modemStatusResponse);

    // pass pointer array to subclass
    modem->setFrameData(getFrameData());
    setCommon(modemStatusResponse);

}

// copy common fields from xbee response to target response
void XBeeResponse::setCommon(XBeeResponse &target)
{
    target.setApiId(getApiId());
    target.setAvailable(isAvailable());
    target.setChecksum(getChecksum());
    target.setErrorCode(getErrorCode());
    target.setFrameLength(getFrameDataLength());
    target.setMsbLength(getMsbLength());
    target.setLsbLength(getLsbLength());
}
