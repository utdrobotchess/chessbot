#include <XBeeRequest.h>

/*-----------------------------------------------------------------------------------------
 XBeeRequest Function Members
 -----------------------------------------------------------------------------------------*/

XBeeRequest::XBeeRequest(uint8_t apiId, uint8_t frameId) 
{
	_apiId = apiId;
	_frameId = frameId;
}

void XBeeRequest::setFrameId(uint8_t frameId) 
{
	_frameId = frameId;
}

uint8_t XBeeRequest::getFrameId() 
{
	return _frameId;
}

uint8_t XBeeRequest::getApiId() 
{
	return _apiId;
}

void XBeeRequest::setApiId(uint8_t apiId) 
{
	_apiId = apiId;
}
/*-----------------------------------------------------------------------------------------
 PayloadRequest Function Members
 -----------------------------------------------------------------------------------------*/

PayloadRequest::PayloadRequest(uint8_t apiId, uint8_t frameId, uint8_t *payload, uint8_t payloadLength) : XBeeRequest(apiId, frameId) 
{
	_payloadPtr = payload;
	_payloadLength = payloadLength;
}

uint8_t* PayloadRequest::getPayload() 
{
	return _payloadPtr;
}

void PayloadRequest::setPayload(uint8_t* payload) 
{
	_payloadPtr = payload;
}

uint8_t PayloadRequest::getPayloadLength() 
{
	return _payloadLength;
}

void PayloadRequest::setPayloadLength(uint8_t payloadLength) 
{
	_payloadLength = payloadLength;
}

/*-----------------------------------------------------------------------------------------
 ZBTxRequest Function Members
 -----------------------------------------------------------------------------------------*/

ZBTxRequest::ZBTxRequest() : PayloadRequest(ZB_TX_REQUEST, DEFAULT_FRAME_ID, NULL, 0) {}

ZBTxRequest::ZBTxRequest(XBeeAddress64 &addr64, uint8_t *data, uint8_t dataLength): PayloadRequest(ZB_TX_REQUEST, DEFAULT_FRAME_ID, data, dataLength) 
{
	_addr64 = addr64;
	_addr16 = ZB_BROADCAST_ADDRESS;
	_broadcastRadius = ZB_BROADCAST_RADIUS_MAX_HOPS;
	_option = ZB_TX_UNICAST;
}
ZBTxRequest::ZBTxRequest(XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId): PayloadRequest(ZB_TX_REQUEST, frameId, data, dataLength) 
{
	_addr64 = addr64;
	_addr16 = addr16;
	_broadcastRadius = broadcastRadius;
	_option = option;
}

XBeeAddress64& ZBTxRequest::getAddress64() 
{
	return _addr64;
}

void ZBTxRequest::setAddress64(XBeeAddress64& addr64) 
{
	_addr64 = addr64;
}

uint16_t ZBTxRequest::getAddress16() 
{
	return _addr16;
}

void ZBTxRequest::setAddress16(uint16_t addr16) 
{
	_addr16 = addr16;
}

uint8_t ZBTxRequest::getBroadcastRadius() 
{
	return _broadcastRadius;
}

void ZBTxRequest::setBroadcastRadius(uint8_t broadcastRadius) 
{
	_broadcastRadius = broadcastRadius;
}

uint8_t ZBTxRequest::getOption() 
{
	return _option;
}

void ZBTxRequest::setOption(uint8_t option) 
{
	_option = option;
}

uint8_t ZBTxRequest::getFrameData(uint8_t pos) 
{
	switch(pos)
	{
		case 0:
			return (_addr64.getMsb() >> 24) & 0xff;
			break;
		case 1:
			return (_addr64.getMsb() >> 16) & 0xff;
			break;
		case 2:
			return (_addr64.getMsb() >> 8) & 0xff;
			break;
		case 3:
			return _addr64.getMsb() & 0xff;
			break;
		case 4:
			return (_addr64.getLsb() >> 24) & 0xff;
			break;
		case 5:
			return (_addr64.getLsb() >> 16) & 0xff;
			break;
		case 6:
			return (_addr64.getLsb() >> 8) & 0xff;
			break;
		case 7:
			return _addr64.getLsb() & 0xff;
			break;
		case 8:
			return (_addr16 >> 8) & 0xff;
			break;
		case 9:
			return _addr16 & 0xff;
			break;
		case 10:
			return _broadcastRadius;
			break;
		case 11:
			return _option;
			break;
		default:
			return getPayload()[pos - ZB_TX_API_LENGTH];
			break;
	}
}

uint8_t ZBTxRequest::getFrameDataLength() 
{
	return ZB_TX_API_LENGTH + getPayloadLength();
}

/*-----------------------------------------------------------------------------------------
 AtCommandRequest Function Members
 -----------------------------------------------------------------------------------------*/

 AtCommandRequest::AtCommandRequest() : XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID) 
{
	_command = NULL;
	clearCommandValue();
}

AtCommandRequest::AtCommandRequest(uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) : XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID) 
{
	_command = command;
	_commandValue = commandValue;
	_commandValueLength = commandValueLength;
}

AtCommandRequest::AtCommandRequest(uint8_t *command) : XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID) 
{
	_command = command;
	clearCommandValue();
}

uint8_t AtCommandRequest::getFrameData(uint8_t pos) 
{

	if(pos == 0) 
		return _command[0];

	else if(pos == 1) 
		return _command[1];

	else 
		return _commandValue[pos - AT_COMMAND_API_LENGTH];
}

uint8_t AtCommandRequest::getFrameDataLength() 
{
	// command is 2 byte + length of value
	return AT_COMMAND_API_LENGTH + _commandValueLength;
}

uint8_t* AtCommandRequest::getCommand() 
{
	return _command;
}

void AtCommandRequest::setCommand(uint8_t* command) 
{
	_command = command;
}

uint8_t* AtCommandRequest::getCommandValue() 
{
	return _commandValue;
}

void AtCommandRequest::setCommandValue(uint8_t* value) 
{
	_commandValue = value;
}

void AtCommandRequest::clearCommandValue() 
{
	_commandValue = NULL;
	_commandValueLength = 0;
}

uint8_t AtCommandRequest::getCommandValueLength() 
{
	return _commandValueLength;
}

void AtCommandRequest::setCommandValueLength(uint8_t length) 
{
	_commandValueLength = length;
}

/*-----------------------------------------------------------------------------------------
 RemoteAtCommandRequest Function Members
 -----------------------------------------------------------------------------------------*/

 XBeeAddress64 RemoteAtCommandRequest::broadcastAddress64 = XBeeAddress64(0x0, BROADCAST_ADDRESS);

RemoteAtCommandRequest::RemoteAtCommandRequest() : AtCommandRequest(NULL, NULL, 0) 
{
	_remoteAddress16 = 0;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) : AtCommandRequest(command, commandValue, commandValueLength) 
{
	_remoteAddress64 = broadcastAddress64;
	_remoteAddress16 = remoteAddress16;
	_applyChanges = true;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command) : AtCommandRequest(command, NULL, 0) 
{
	_remoteAddress64 = broadcastAddress64;
	_remoteAddress16 = remoteAddress16;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) : AtCommandRequest(command, commandValue, commandValueLength) 
{
	_remoteAddress64 = remoteAddress64;
	// don't worry.. works for series 1 too!
	_remoteAddress16 = ZB_BROADCAST_ADDRESS;
	_applyChanges = true;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command) : AtCommandRequest(command, NULL, 0) 
{
	_remoteAddress64 = remoteAddress64;
	_remoteAddress16 = ZB_BROADCAST_ADDRESS;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}

uint16_t RemoteAtCommandRequest::getRemoteAddress16() 
{
	return _remoteAddress16;
}

void RemoteAtCommandRequest::setRemoteAddress16(uint16_t remoteAddress16) 
{
	_remoteAddress16 = remoteAddress16;
}

XBeeAddress64& RemoteAtCommandRequest::getRemoteAddress64() 
{
	return _remoteAddress64;
}

void RemoteAtCommandRequest::setRemoteAddress64(XBeeAddress64 &remoteAddress64) 
{
	_remoteAddress64 = remoteAddress64;
}

bool RemoteAtCommandRequest::getApplyChanges() 
{
	return _applyChanges;
}

void RemoteAtCommandRequest::setApplyChanges(bool applyChanges) 
{
	_applyChanges = applyChanges;
}


uint8_t RemoteAtCommandRequest::getFrameData(uint8_t pos) 
{
	switch(pos)
	{
		case 0:
			return (_remoteAddress64.getMsb() >> 24) & 0xff;
			break;
		case 1:
			return (_remoteAddress64.getMsb() >> 16) & 0xff;
			break;
		case 2:
			return (_remoteAddress64.getMsb() >> 8) & 0xff;
			break;
		case 3:
			return _remoteAddress64.getMsb() & 0xff;
			break;
		case 4:
			return (_remoteAddress64.getLsb() >> 24) & 0xff;
			break;
		case 5:
			return (_remoteAddress64.getLsb() >> 16) & 0xff;
			break;
		case 6:
			return(_remoteAddress64.getLsb() >> 8) & 0xff;
			break;
		case 7:
			return _remoteAddress64.getLsb() & 0xff;
			break;
		case 8:
			return (_remoteAddress16 >> 8) & 0xff;
			break;
		case 9:
			return _remoteAddress16 & 0xff;
			break;
		case 10:
			return _applyChanges ? 2: 0;
			break;
		case 11:
			return getCommand()[0];
			break;
		case 12:
			return getCommand()[1];
			break;
		default:
			return getCommandValue()[pos - REMOTE_AT_COMMAND_API_LENGTH];
			break;
	}
}

uint8_t RemoteAtCommandRequest::getFrameDataLength() 
{
	return REMOTE_AT_COMMAND_API_LENGTH + getCommandValueLength();
}