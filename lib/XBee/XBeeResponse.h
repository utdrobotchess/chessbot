#ifndef XBEE_RESPONSE_H
#define XBEE_RESPONSE_H

#include <Arduino.h>
#include <XbeeAddress.h>

#define AT_OK 0
#define	SUCCESS 0x0
#define NO_ERROR 0

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class XBeeResponse
{
public:
	XBeeResponse();
	void init();
	void reset();

	/*
	  Returns true if the response has been successfully parsed and is complete and ready for use
	 */
	bool isAvailable();
	void setAvailable(bool complete);

	bool isError();

	/*
	  Returns an error code, or zero, if successful.
	  Error codes include: CHECKSUM_FAILURE, PACKET_EXCEEDS_BYTE_ARRAY_LENGTH, UNEXPECTED_START_BYTE
	 */
	uint8_t getErrorCode();
	void setErrorCode(uint8_t errorCode);

	uint8_t getApiId();
	void setApiId(uint8_t apiId);

	uint8_t getMsbLength();
	void setMsbLength(uint8_t msbLength);

	uint8_t getLsbLength();
	void setLsbLength(uint8_t lsbLength);

	uint8_t getChecksum();
	void setChecksum(uint8_t checksum);

	/*
	  Returns the length of the frame data: all bytes after the api id, and prior to the checksum
	  Note up to release 0.1.2, this was incorrectly including the checksum in the length.
	 */
	uint8_t getFrameDataLength();
	void setFrameLength(uint8_t frameLength);

	/*
	  Returns the buffer that contains the response.
	  Starts with byte that follows API ID and includes all bytes prior to the checksum
	  Length is specified by getFrameDataLength()
	  Note: Unlike Digi's definition of the frame data, this does not start with the API ID..
	  The reason for this is all responses include an API ID, whereas my frame data
	  includes only the API specific data.
	 */
	uint8_t* getFrameData();
	void setFrameData(uint8_t* frameDataPtr);

	uint16_t getPacketLength();

	/*
	  Call with instance of ZBTxStatusResponse class only if getApiId() == ZB_TX_STATUS_RESPONSE
	  to populate response
	 */
	void getZBTxStatusResponse(XBeeResponse &response);

	/*
	  Call with instance of ZBRxResponse class only if getApiId() == ZB_RX_RESPONSE
	  to populate response
	 */
	void getZBRxResponse(XBeeResponse &response);

	/*
	  Call with instance of ZBRxIoSampleResponse class only if getApiId() == ZB_IO_SAMPLE_RESPONSE
	  to populate response
	 */
	void getZBRxIoSampleResponse(XBeeResponse &response);

	/*
	  Call with instance of AtCommandResponse only if getApiId() == AT_COMMAND_RESPONSE
	 */
	void getAtCommandResponse(XBeeResponse &responses);

	/*
	  Call with instance of RemoteAtCommandResponse only if getApiId() == REMOTE_AT_COMMAND_RESPONSE
	 */
	void getRemoteAtCommandResponse(XBeeResponse &response);

	/*
	  Call with instance of ModemStatusResponse only if getApiId() == MODEM_STATUS_RESPONSE
	 */
	void getModemStatusResponse(XBeeResponse &response);

protected:
	uint8_t* _frameDataPtr;

private:
	void setCommon(XBeeResponse &target);
	uint8_t _apiId;
	uint8_t _msbLength;
	uint8_t _lsbLength;
	uint8_t _checksum;
	uint8_t _frameLength;
	bool 	_complete;
	uint8_t _errorCode;
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class ModemStatusResponse : public XBeeResponse
{
public:
	ModemStatusResponse();
	uint8_t getStatus();
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class RxDataResponse : public XBeeResponse
{
public:
	RxDataResponse();

	/*
	  Returns the specified index of the payload.  The index may be 0 to getDataLength() - 1
	  This method is deprecated; use uint8_t* getData()
	 */
	uint8_t getData(int index);

	/*
	  Returns the payload array.  This may be accessed from index 0 to getDataLength() - 1
	 */
	uint8_t* getData();

	/*
	  Returns the length of the payload
	 */
	virtual uint8_t getDataLength() = 0;

	/*
	  Returns the position in the frame data where the data begins
	 */
	virtual uint8_t getDataOffset() = 0;
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class ZBRxResponse : public RxDataResponse
{
public:
	ZBRxResponse();
	XBeeAddress64& getRemoteAddress64();
	uint16_t getRemoteAddress16();
	uint8_t getOption();
	uint8_t getDataLength();
	uint8_t getDataOffset();

private:
	XBeeAddress64 _remoteAddress64;
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class ZBRxIoSampleResponse : public ZBRxResponse
{
public:
	ZBRxIoSampleResponse();
	bool containsAnalog();
	bool containsDigital();

	bool isAnalogEnabled(uint8_t pin);
	bool isDigitalEnabled(uint8_t pin);
	bool isDigitalOn(uint8_t pin);

	uint16_t getAnalog(uint8_t pin);

	uint8_t getAnalogMask();
	uint8_t getDigitalMaskMsb();
	uint8_t getDigitalMaskLsb();
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class FrameIdResponse : public XBeeResponse
{
public:
	FrameIdResponse();
	uint8_t getFrameId();

private:
	uint8_t _frameId;
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class ZBTxStatusResponse : public FrameIdResponse
{
public:
    ZBTxStatusResponse();
    uint16_t getRemoteAddress();
    uint8_t getTxRetryCount();
    uint8_t getDeliveryStatus();
    uint8_t getDiscoveryStatus();
    bool isSuccess();
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class AtCommandResponse : public FrameIdResponse
{
public:
    AtCommandResponse();
    uint8_t* getCommand();
    uint8_t getStatus();
    uint8_t* getValue();
    uint8_t getValueLength();
    bool isOk();
};

/*-----------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------*/
class RemoteAtCommandResponse : public AtCommandResponse
{
public:
    RemoteAtCommandResponse();
    uint8_t* getCommand();
    uint8_t getStatus();
    uint8_t* getValue();
    uint8_t getValueLength();
    uint16_t getRemoteAddress16();
    XBeeAddress64& getRemoteAddress64();
    bool isOk();

private:
    XBeeAddress64 _remoteAddress64;
};

#endif