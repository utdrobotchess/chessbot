#ifndef XBEE_REQUEST_H
#define XBEE_REQUEST_H

#include <Arduino.h>
#include "XbeeAddress64.h"

#define DEFAULT_FRAME_ID 0

#define AT_COMMAND_API_LENGTH 2
#define ZB_TX_API_LENGTH 12
#define REMOTE_AT_COMMAND_API_LENGTH 13

#define ZB_TX_UNICAST 0
#define BROADCAST_ADDRESS 0xffff
#define ZB_BROADCAST_ADDRESS 0xfffe
#define ZB_BROADCAST_RADIUS_MAX_HOPS 0

#define ZB_TX_REQUEST 0x10

/*-----------------------------------------------------------------------------------------
  Super class of all XBee requests (TX packets)
  Users should never create an instance of this class; instead use an subclass of this class
  It is recommended to reuse Subclasses of the class to conserve memory
  -----------------------------------------------------------------------------------------*/
class XBeeRequest
{
    public:
        XBeeRequest(uint8_t apiId, uint8_t frameId)
        { _apiId = apiId; _frameId = frameId; }

        void setFrameId(uint8_t frameId) // 1 < frameId < 255 to recieve a TX status response
        { _frameId = frameId; }

        uint8_t getFrameId()
        { return _frameId; }

        uint8_t getApiId()
        { return _apiId; }

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
        void setApiId(uint8_t apiId)
        { _apiId = apiId; }

    private:
        uint8_t _apiId;
        uint8_t _frameId;
};

/*-----------------------------------------------------------------------------------------
  -----------------------------------------------------------------------------------------*/
class PayloadRequest : public XBeeRequest
{
    public:
        PayloadRequest(uint8_t apiId, uint8_t frameId, uint8_t *payload, uint8_t payloadLength) : XBeeRequest(apiId, frameId)
    { _payloadPtr = payload; _payloadLength = payloadLength; }

        uint8_t* getPayload()
        { return _payloadPtr; }

        void setPayload(uint8_t* payloadPtr)
        { _payloadPtr = payloadPtr; }

        uint8_t getPayloadLength()
        { return _payloadLength; }

        /*
           Sets the length of the payload to include in the request.  For example if the payload array
           is 50 bytes and you only want the first 10 to be included in the packet, set the length to 10.
           Length must be <= to the array length.
           */
        void setPayloadLength(uint8_t payloadLength)
        { _payloadLength = payloadLength; }

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
        ZBTxRequest() : PayloadRequest(ZB_TX_REQUEST, DEFAULT_FRAME_ID, NULL, 0){}

        /*
           Creates a unicast ZBTxRequest with the ACK option and DEFAULT_FRAME_ID
           */
        ZBTxRequest(XBeeAddress64 &addr64, uint8_t *payload, uint8_t payloadLength) : PayloadRequest(ZB_TX_REQUEST, DEFAULT_FRAME_ID, payload, payloadLength)
    { _addr64 = addr64; _addr16 = ZB_BROADCAST_ADDRESS; _broadcastRadius = ZB_BROADCAST_RADIUS_MAX_HOPS; _option = ZB_TX_UNICAST; }

        ZBTxRequest(XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId) : PayloadRequest(ZB_TX_REQUEST, frameId, payload, payloadLength)
    { _addr64 = addr64; _addr16 = addr16; _broadcastRadius = broadcastRadius; _option = option; }

        XBeeAddress64& getAddress64()
        { return _addr64; }

        void setAddress64(XBeeAddress64& addr64)
        { _addr64 = addr64; }

        uint16_t getAddress16()
        { return _addr16; }

        void setAddress16(uint16_t addr16)
        { _addr16 = addr16; }

        uint8_t getBroadcastRadius()
        { return _broadcastRadius; }

        void setBroadcastRadius(uint8_t broadcastRadius)
        { _broadcastRadius = broadcastRadius; }

        uint8_t getOption()
        { return _option; }

        void setOption(uint8_t option)
        { _option = option; }

    protected:
        uint8_t getFrameData(uint8_t pos)
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

        uint8_t getFrameDataLength()
        { return ZB_TX_API_LENGTH + getPayloadLength(); }

    private:
        XBeeAddress64 _addr64;
        uint16_t _addr16;
        uint8_t _broadcastRadius;
        uint8_t _option;
};

#endif //XBEE_REQUEST_H
