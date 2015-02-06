#ifndef XBEE_REQUEST_H
#define XBEE_REQUEST_H

#include <Arduino.h>
#include <XbeeAddress.h>

#define DEFAULT_FRAME_ID 1

#define AT_COMMAND_API_LENGTH 2
#define ZB_TX_API_LENGTH 12
#define REMOTE_AT_COMMAND_API_LENGTH 13

#define ZB_TX_UNICAST 0
#define BROADCAST_ADDRESS 0xffff
#define ZB_BROADCAST_ADDRESS 0xfffe
#define ZB_BROADCAST_RADIUS_MAX_HOPS 0

#define ZB_TX_REQUEST 0x10
#define REMOTE_AT_REQUEST 0x17
#define AT_COMMAND_REQUEST 0x08

/*-----------------------------------------------------------------------------------------
  Super class of all XBee requests (TX packets)
  Users should never create an instance of this class; instead use an subclass of this class
  It is recommended to reuse Subclasses of the class to conserve memory
 -----------------------------------------------------------------------------------------*/
class XBeeRequest
{
public:
	XBeeRequest(uint8_t apiId, uint8_t frameId);

	/*
	  Sets the frame id.  Must be between 1 and 255 inclusive to get a TX status response.
	 */
	void setFrameId(uint8_t frameId);

	uint8_t getFrameId();
	uint8_t getApiId();

	/*
	  Starting after the frame id (pos = 0) and up to but not including the checksum
	  Note: Unlike Digi's definition of the frame data, this does not start with the API ID.
	  The reason for this is the API ID and Frame ID are common to all requests, whereas my definition of
	  frame data is only the API specific data.
	 */
	virtual uint8_t getFrameData(uint8_t pos) = 0;

	/*
	  Returns the size of the api frame (not including frame id or api id or checksum).
	 */
	virtual uint8_t getFrameDataLength() = 0;

protected:
	void setApiId(uint8_t apiId);

private:
	uint8_t _apiId;
	uint8_t _frameId;
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class PayloadRequest : public XBeeRequest
{
public:
	PayloadRequest(uint8_t apiId, uint8_t frameId, uint8_t *payload, uint8_t payloadLength);

	/*
	  Returns the payload of the packet, if not null
	 */
	uint8_t* getPayload();

	/*
	  Sets the payload array
	 */
	void setPayload(uint8_t* payloadPtr);

	/*
	  Returns the length of the payload array, as specified by the user.
	 */
	uint8_t getPayloadLength();

	/*
	  Sets the length of the payload to include in the request.  For example if the payload array
	  is 50 bytes and you only want the first 10 to be included in the packet, set the length to 10.
	  Length must be <= to the array length.
	 */
	void setPayloadLength(uint8_t payloadLength);

private:
	uint8_t* _payloadPtr;
	uint8_t _payloadLength;
};

/*-----------------------------------------------------------------------------------------
  Represents a Series 2 TX packet that corresponds to Api Id: ZB_TX_REQUEST
 
  Be careful not to send a data array larger than the max packet size of your radio.
  This class does not perform any validation of packet size and there will be no indication
  if the packet is too large, other than you will not get a TX Status response.
  The datasheet says 72 bytes is the maximum for ZNet firmware and ZB Pro firmware provides
  the ATNP command to get the max supported payload size.  This command is useful since the
  maximum payload size varies according to certain settings, such as encryption.
  ZB Pro firmware provides a PAYLOAD_TOO_LARGE that is returned if payload size
  exceeds the maximum.
 -----------------------------------------------------------------------------------------*/
class ZBTxRequest : public PayloadRequest
{
public:
	/*
	  Creates a default instance of this class.  At a minimum you must specify
	  a payload, payload length and a destination address before sending this request.
	 */
	ZBTxRequest();

	/*
	  Creates a unicast ZBTxRequest with the ACK option and DEFAULT_FRAME_ID
	 */
	ZBTxRequest(XBeeAddress64 &addr64, uint8_t *payload, uint8_t payloadLength);

	ZBTxRequest(XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId);
	
	XBeeAddress64& getAddress64();
	void setAddress64(XBeeAddress64& addr64);

	uint16_t getAddress16();
	void setAddress16(uint16_t addr16);

	uint8_t getBroadcastRadius();
	void setBroadcastRadius(uint8_t broadcastRadius);

	uint8_t getOption();
	void setOption(uint8_t option);

protected:
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();

private:
	XBeeAddress64 _addr64;
	uint16_t _addr16;
	uint8_t _broadcastRadius;
	uint8_t _option;
};

/*-----------------------------------------------------------------------------------------
  Represents an AT Command TX packet
  The command is used to configure the serially connected XBee radio
 -----------------------------------------------------------------------------------------*/
class AtCommandRequest : public XBeeRequest
{
public:
	AtCommandRequest();
	AtCommandRequest(uint8_t *command);
	AtCommandRequest(uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength);

	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();

	uint8_t* getCommand();
	void setCommand(uint8_t* command);

	uint8_t* getCommandValue();
	void setCommandValue(uint8_t* command);
	void clearCommandValue(); // Clears the optional commandValue and commandValueLength so that a query may be sent

	uint8_t getCommandValueLength();
	void setCommandValueLength(uint8_t length);
	
private:
	uint8_t *_command;
	uint8_t *_commandValue;
	uint8_t _commandValueLength;
};

/*-----------------------------------------------------------------------------------------
  Represents a Remote AT Command TX packet
  The command is used to configure a remote XBee radio
 -----------------------------------------------------------------------------------------*/
class RemoteAtCommandRequest : public AtCommandRequest
{
public:
	RemoteAtCommandRequest();

	/*
	  Creates a RemoteAtCommandRequest with 16-bit address to set a command.
	  64-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength);

	/*
	  Creates a RemoteAtCommandRequest with 16-bit address to query a command.
	  64-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command);

	/*
	  Creates a RemoteAtCommandRequest with 64-bit address to set a command.
	  16-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength);

	/*
	  Creates a RemoteAtCommandRequest with 16-bit address to query a command.
	  16-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command);

	uint16_t getRemoteAddress16();
	void setRemoteAddress16(uint16_t remoteAddress16);

	XBeeAddress64& getRemoteAddress64();
	void setRemoteAddress64(XBeeAddress64 &remoteAddress64);

	bool getApplyChanges();
	void setApplyChanges(bool applyChanges);

	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();

	static XBeeAddress64 broadcastAddress64;

private:
	XBeeAddress64 _remoteAddress64;
	uint16_t _remoteAddress16;
	bool _applyChanges;
};

#endif //XBEE_REQUEST_H