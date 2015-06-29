#ifndef XBEE_RESPONSE_H
#define XBEE_RESPONSE_H

#include <Arduino.h>
#include "XBeeAddress64.h"

#define AT_OK 0
#define	SUCCESS 0x0
#define NO_ERROR 0

/*-----------------------------------------------------------------------------------------
  -----------------------------------------------------------------------------------------*/
class XBeeResponse
{
    public:
        XBeeResponse(){}

        void init()
        { _complete = false; _errorCode = NO_ERROR; _checksum = 0; }

        void reset()
        { init(); _apiId = 0; _msbLength = 0; _lsbLength = 0; _checksum = 0; _frameLength = 0; _errorCode = NO_ERROR; }

        /*
           Returns true if the response has been successfully parsed and is complete and ready for use
           */
        bool isAvailable()
        { return _complete; }

        void setAvailable(bool complete)
        { _complete = complete; }

        bool isError()
        { return _errorCode > 0; }

        /*
           Returns an error code, or zero, if successful.
           Error codes include: CHECKSUM_FAILURE, PACKET_EXCEEDS_BYTE_ARRAY_LENGTH, UNEXPECTED_START_BYTE
           */
        uint8_t getErrorCode()
        { return _errorCode; }

        void setErrorCode(uint8_t errorCode)
        { _errorCode = errorCode; }

        uint8_t getApiId()
        { return _apiId; }

        void setApiId(uint8_t apiId)
        { _apiId = apiId; }

        uint8_t getMsbLength()
        { return _msbLength; }

        void setMsbLength(uint8_t msbLength)
        { _msbLength = msbLength; }

        uint8_t getLsbLength()
        { return _lsbLength; }

        void setLsbLength(uint8_t lsbLength)
        { _lsbLength = lsbLength; }

        uint8_t getChecksum()
        { return _checksum; }

        void setChecksum(uint8_t checksum)
        { _checksum = checksum; }

        /*
           Returns the length of the frame data: all bytes after the api id, and prior to the checksum
           Note up to release 0.1.2, this was incorrectly including the checksum in the length.
           */
        uint8_t getFrameDataLength()
        { return _frameLength; }

        void setFrameLength(uint8_t frameLength)
        { _frameLength = frameLength; }

        /*
           Returns the buffer that contains the response.
           Starts with byte that follows API ID and includes all bytes prior to the checksum
           Length is specified by getFrameDataLength()
            Note: Unlike Digi's definition of the frame data, this does not start with the API ID..
            The reason for this is all responses include an API ID, whereas my frame data
            includes only the API specific data.
            */
        uint8_t* getFrameData()
        { return _frameDataPtr; }

        void setFrameData(uint8_t* frameDataPtr)
        { _frameDataPtr = frameDataPtr; }

        uint16_t getPacketLength()
        { return ((_msbLength << 8) & 0xff) + (_lsbLength & 0xff); }

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
        ModemStatusResponse(){}
        uint8_t getStatus()
        { return getFrameData()[0]; }
};

/*-----------------------------------------------------------------------------------------
  -----------------------------------------------------------------------------------------*/
class RxDataResponse : public XBeeResponse
{
    public:
        RxDataResponse(){}

        /*
           Returns the specified index of the payload.  The index may be 0 to getDataLength() - 1
           This method is deprecated; use uint8_t* getData()
           */
        uint8_t getData(int index)
        { return getFrameData()[getDataOffset() + index]; }

        /*
           Returns the payload array.  This may be accessed from index 0 to getDataLength() - 1
           */
        uint8_t* getData()
        { return getFrameData() + getDataOffset(); }

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
        ZBRxResponse() : RxDataResponse()
    { _remoteAddress64 = XBeeAddress64(); }

        XBeeAddress64& getRemoteAddress64()
        { return _remoteAddress64; }

        uint16_t getRemoteAddress16()
        { return 	(getFrameData()[8] << 8) + getFrameData()[9]; }

        uint8_t getOption()
        { return getFrameData()[10]; }

        uint8_t getDataLength()
        { return getPacketLength() - getDataOffset() - 1; }

        uint8_t getDataOffset()
        { return 11; }

    private:
        XBeeAddress64 _remoteAddress64;
};

/*-----------------------------------------------------------------------------------------
  -----------------------------------------------------------------------------------------*/
class FrameIdResponse : public XBeeResponse
{
    public:
        FrameIdResponse(){}
        uint8_t getFrameId()
        { return getFrameData()[0]; }

    private:
        uint8_t _frameId;
};

/*-----------------------------------------------------------------------------------------
  -----------------------------------------------------------------------------------------*/
class ZBTxStatusResponse : public FrameIdResponse
{
    public:
        ZBTxStatusResponse(){}

        uint16_t getRemoteAddress()
        { return  (getFrameData()[1] << 8) + getFrameData()[2]; }

        uint8_t getTxRetryCount()
        { return getFrameData()[3]; }

        uint8_t getDeliveryStatus()
        { return getFrameData()[4]; }

        uint8_t getDiscoveryStatus()
        { return getFrameData()[5]; }

        bool isSuccess()
        { return getDeliveryStatus() == SUCCESS; }
};

#endif
