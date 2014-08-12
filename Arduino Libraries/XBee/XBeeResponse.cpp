#include <XBeeResponse.h>

/*-----------------------------------------------------------------------------------------
 XBeeResponse Function Members
 -----------------------------------------------------------------------------------------*/

XBeeResponse::XBeeResponse() {}

void XBeeResponse::init() 
{
	_complete = false;
	_errorCode = NO_ERROR;
	_checksum = 0;
}

void XBeeResponse::reset() 
{
	init();
	_apiId = 0;
	_msbLength = 0;
	_lsbLength = 0;
	_checksum = 0;
	_frameLength = 0;

	_errorCode = NO_ERROR;
}

bool XBeeResponse::isAvailable() 
{
	return _complete;
}

void XBeeResponse::setAvailable(bool complete) 
{
	_complete = complete;
}

bool XBeeResponse::isError() 
{
	return _errorCode > 0;
}

uint8_t XBeeResponse::getErrorCode() 
{
	return _errorCode;
}

void XBeeResponse::setErrorCode(uint8_t errorCode) 
{
	_errorCode = errorCode;
}

uint8_t XBeeResponse::getApiId() 
{
	return _apiId;
}

void XBeeResponse::setApiId(uint8_t apiId) 
{
	_apiId = apiId;
}

uint8_t XBeeResponse::getMsbLength() 
{
	return _msbLength;
}

void XBeeResponse::setMsbLength(uint8_t msbLength) 
{
	_msbLength = msbLength;
}

uint8_t XBeeResponse::getLsbLength() 
{
	return _lsbLength;
}

void XBeeResponse::setLsbLength(uint8_t lsbLength) 
{
	_lsbLength = lsbLength;
}

uint8_t XBeeResponse::getChecksum() 
{
	return _checksum;
}

void XBeeResponse::setChecksum(uint8_t checksum) 
{
	_checksum = checksum;
}

uint8_t XBeeResponse::getFrameDataLength() 
{
	return _frameLength;
}

void XBeeResponse::setFrameLength(uint8_t frameLength) 
{
	_frameLength = frameLength;
}

uint8_t* XBeeResponse::getFrameData() 
{
	return _frameDataPtr;
}

void XBeeResponse::setFrameData(uint8_t* frameDataPtr) 
{
	_frameDataPtr = frameDataPtr;
}

uint16_t XBeeResponse::getPacketLength() 
{
	return ((_msbLength << 8) & 0xff) + (_lsbLength & 0xff);
}

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

void XBeeResponse::getZBRxIoSampleResponse(XBeeResponse &response) {
	ZBRxIoSampleResponse* zb = static_cast<ZBRxIoSampleResponse*>(&response);


	// pass pointer array to subclass
	zb->setFrameData(getFrameData());
	setCommon(response);

	zb->getRemoteAddress64().setMsb((uint32_t(getFrameData()[0]) << 24) + (uint32_t(getFrameData()[1]) << 16) + (uint16_t(getFrameData()[2]) << 8) + getFrameData()[3]);
	zb->getRemoteAddress64().setLsb((uint32_t(getFrameData()[4]) << 24) + (uint32_t(getFrameData()[5]) << 16) + (uint16_t(getFrameData()[6]) << 8) + (getFrameData()[7]));
}

void XBeeResponse::getAtCommandResponse(XBeeResponse &atCommandResponse) {

	AtCommandResponse* at = static_cast<AtCommandResponse*>(&atCommandResponse);

	// pass pointer array to subclass
	at->setFrameData(getFrameData());
	setCommon(atCommandResponse);
}

void XBeeResponse::getRemoteAtCommandResponse(XBeeResponse &response) {

	// TODO no real need to cast.  change arg to match expected class
	RemoteAtCommandResponse* at = static_cast<RemoteAtCommandResponse*>(&response);

	// pass pointer array to subclass
	at->setFrameData(getFrameData());
	setCommon(response);

	at->getRemoteAddress64().setMsb((uint32_t(getFrameData()[1]) << 24) + (uint32_t(getFrameData()[2]) << 16) + (uint16_t(getFrameData()[3]) << 8) + getFrameData()[4]);
	at->getRemoteAddress64().setLsb((uint32_t(getFrameData()[5]) << 24) + (uint32_t(getFrameData()[6]) << 16) + (uint16_t(getFrameData()[7]) << 8) + (getFrameData()[8]));

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

/*-----------------------------------------------------------------------------------------
 ModemStatusResponse Function Members
 -----------------------------------------------------------------------------------------*/

ModemStatusResponse::ModemStatusResponse() {}

uint8_t ModemStatusResponse::getStatus() 
{
	return getFrameData()[0];
}

/*-----------------------------------------------------------------------------------------
 RxDataResponse Function Members
 -----------------------------------------------------------------------------------------*/

RxDataResponse::RxDataResponse() : XBeeResponse() {}

uint8_t RxDataResponse::getData(int index) 
{
	return getFrameData()[getDataOffset() + index];
}

uint8_t* RxDataResponse::getData() 
{
	return getFrameData() + getDataOffset();
}

/*-----------------------------------------------------------------------------------------
 ZBRxResponse Function Members
 -----------------------------------------------------------------------------------------*/

ZBRxResponse::ZBRxResponse(): RxDataResponse() 
{
	_remoteAddress64 = XBeeAddress64();
}

XBeeAddress64& ZBRxResponse::getRemoteAddress64() 
{
	return _remoteAddress64;
}

uint16_t ZBRxResponse::getRemoteAddress16() 
{
	return 	(getFrameData()[8] << 8) + getFrameData()[9];
}

uint8_t ZBRxResponse::getOption() 
{
	return getFrameData()[10];
}

uint8_t ZBRxResponse::getDataLength() 
{
	return getPacketLength() - getDataOffset() - 1;
}

// markers to read data from packet array.  this is the index, so the 12th item in the array
uint8_t ZBRxResponse::getDataOffset() 
{	
	return 11;
}

/*-----------------------------------------------------------------------------------------
 ZBRxIoSampleResponse Function Members
 -----------------------------------------------------------------------------------------*/

ZBRxIoSampleResponse::ZBRxIoSampleResponse() : ZBRxResponse() {}

bool ZBRxIoSampleResponse::containsAnalog() 
{
	return getAnalogMask() > 0;
}

bool ZBRxIoSampleResponse::containsDigital() 
{
	return getDigitalMaskMsb() > 0 || getDigitalMaskLsb() > 0;
}

bool ZBRxIoSampleResponse::isAnalogEnabled(uint8_t pin) 
{
	return ((getAnalogMask() >> pin) & 1) == 1;
}

bool ZBRxIoSampleResponse::isDigitalEnabled(uint8_t pin) 
{
	if (pin <= 7) 
		return ((getDigitalMaskLsb() >> pin) & 1) == 1; // added extra parens to calm avr compiler
	else 
		return ((getDigitalMaskMsb() >> (pin - 8)) & 1) == 1;
}

/*-----------------------------------------------------------------------------------------
 FrameIdResponse Function Members
 -----------------------------------------------------------------------------------------*/

FrameIdResponse::FrameIdResponse() {}

uint8_t FrameIdResponse::getFrameId() 
{
	return getFrameData()[0];
}

/*-----------------------------------------------------------------------------------------
 ZBTxStatusResponse Function Members
 -----------------------------------------------------------------------------------------*/

ZBTxStatusResponse::ZBTxStatusResponse() : FrameIdResponse() {}

uint16_t ZBTxStatusResponse::getRemoteAddress() 
{
	return  (getFrameData()[1] << 8) + getFrameData()[2];
}

uint8_t ZBTxStatusResponse::getTxRetryCount() 
{
	return getFrameData()[3];
}

uint8_t ZBTxStatusResponse::getDeliveryStatus() 
{
	return getFrameData()[4];
}

uint8_t ZBTxStatusResponse::getDiscoveryStatus() 
{
	return getFrameData()[5];
}

bool ZBTxStatusResponse::isSuccess() 
{
	return getDeliveryStatus() == SUCCESS;
}

/*-----------------------------------------------------------------------------------------
 AtCommandResponse Function Members
 -----------------------------------------------------------------------------------------*/

AtCommandResponse::AtCommandResponse() {}

uint8_t* AtCommandResponse::getCommand() 
{
	return getFrameData() + 1;
}

uint8_t AtCommandResponse::getStatus() 
{
	return getFrameData()[3];
}

uint8_t* AtCommandResponse::getValue() 
{
	if (getValueLength() > 0) 
		return getFrameData() + 4; // value is only included for query commands.  set commands does not return a value

	return NULL;
}

uint8_t AtCommandResponse::getValueLength() 
{
	return getFrameDataLength() - 4;
}

bool AtCommandResponse::isOk() 
{
	return getStatus() == AT_OK;
}

/*-----------------------------------------------------------------------------------------
 RemoteAtCommandResponse Function Members
 -----------------------------------------------------------------------------------------*/

RemoteAtCommandResponse::RemoteAtCommandResponse() : AtCommandResponse() {}

uint8_t* RemoteAtCommandResponse::getCommand() 
{
	return getFrameData() + 11;
}

uint8_t RemoteAtCommandResponse::getStatus() 
{
	return getFrameData()[13];
}

uint8_t* RemoteAtCommandResponse::getValue() 
{
	if (getValueLength() > 0)		
		return getFrameData() + 14; //value is only included for query commands.  set commands does not return a value

	return NULL;
}

uint8_t RemoteAtCommandResponse::getValueLength() 
{
	return getFrameDataLength() - 14;
}

uint16_t RemoteAtCommandResponse::getRemoteAddress16() 
{
	return uint16_t((getFrameData()[9] << 8) + getFrameData()[10]);
}

XBeeAddress64& RemoteAtCommandResponse::getRemoteAddress64() 
{
	return _remoteAddress64;
}

bool RemoteAtCommandResponse::isOk() 
{
	return getStatus() == AT_OK;
}

bool ZBRxIoSampleResponse::isDigitalOn(uint8_t pin) 
{
	// D0-7
	//DIO LSB is index 5
	if (pin <= 7)  	
		return ((getFrameData()[16] >> pin) & 1) == 1;

	// D10-12
	// DIO MSB is index 4
	else 			
		return ((getFrameData()[15] >> (pin - 8)) & 1) == 1;
}

uint16_t ZBRxIoSampleResponse::getAnalog(uint8_t pin) 
{
	// analog starts 13 bytes after sample size, if no dio enabled
	uint8_t start = 15;

	if (containsDigital()) 
		start+=2; // make room for digital i/o

	// start depends on how many pins before this pin are enabled
	for (int i = 0; i < pin; i++) 
	{
		if (isAnalogEnabled(i)) 
			start+=2;
	}

	return (uint16_t)((getFrameData()[start] << 8) + getFrameData()[start + 1]);
}

// 64 + 16 addresses, sample size, option = 12 (index 11), so this starts at 12
uint8_t ZBRxIoSampleResponse::getDigitalMaskMsb() 
{
	return getFrameData()[12] & 0x1c;
}

uint8_t ZBRxIoSampleResponse::getDigitalMaskLsb() 
{
	return getFrameData()[13];
}

uint8_t ZBRxIoSampleResponse::getAnalogMask() 
{
	return getFrameData()[14] & 0x8f;
}