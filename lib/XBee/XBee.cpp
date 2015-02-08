#include "XBee.h"

XBee::XBee(): _response(XBeeResponse()) 
{
    _pos = 0;
    _escape = false;
    _checksumTotal = 0;
    _nextFrameId = 0;

    _response.init();
    _response.setFrameData(_responseFrameData);
    _serial = &Serial;
}

void XBee::setSerial(Stream &serial) 
{
	_serial = &serial;
}

void XBee::readPacket() 
{
	if (_response.isAvailable() || _response.isError()) 
		resetResponse();

    while (available()) 
    {
        lastByteRead = read();

        if (_pos > 0 && lastByteRead == START_BYTE && ATAP == 2) 
        {
        	// new packet start before previous packeted completed -- discard previous packet and start over
        	_response.setErrorCode(UNEXPECTED_START_BYTE);
        	return;
        }

		if (_pos > 0 && lastByteRead == ESCAPE) 
		{
			if (available()) 
			{
				lastByteRead = read();
				lastByteRead = 0x20 ^ lastByteRead;
			} 
			else 
			{
				_escape = true;
				continue;
			}
		}

		if (_escape == true) 
		{
			lastByteRead = 0x20 ^ lastByteRead;
			_escape = false;
		}

		// checksum includes all bytes starting with api id
		if (_pos >= API_ID_INDEX) 
		{
			_checksumTotal+= lastByteRead;
		}

        switch(_pos) 
        {
			case 0:
		        if (lastByteRead == START_BYTE) 
		        	_pos++;
		        break;
			case 1:
				// length msb
				_response.setMsbLength(lastByteRead);
				_pos++;

				break;
			case 2:
				// length lsb
				_response.setLsbLength(lastByteRead);
				_pos++;

				break;
			case 3:
				_response.setApiId(lastByteRead);
				_pos++;

				break;
			default:
				// starts at fifth byte

				if (_pos > MAX_FRAME_DATA_SIZE) 
				{
					// exceed max size.  should never occur
					_response.setErrorCode(PACKET_EXCEEDS_BYTE_ARRAY_LENGTH);
					return;
				}

				// check if we're at the end of the packet
				// packet length does not include start, length, or checksum bytes, so add 3
				if (_pos == (_response.getPacketLength() + 3)) 
				{
					// verify checksum

					if ((_checksumTotal & 0xff) == 0xff) 
					{
						_response.setChecksum(lastByteRead);
						_response.setAvailable(true);

						_response.setErrorCode(NO_ERROR);
					} 
					else // checksum failed
						_response.setErrorCode(CHECKSUM_FAILURE);

					// minus 4 because we start after start,msb,lsb,api and up to but not including checksum
					// e.g. if frame was one byte, _pos=4 would be the byte, pos=5 is the checksum, where end stop reading
					_response.setFrameLength(_pos - 4);

					// reset state vars
					_pos = 0;

					return;
				} 
				else 
				{
					// add to packet array, starting with the fourth byte of the apiFrame
					_response.getFrameData()[_pos - 4] = lastByteRead;
					_pos++;
				}
        }
    }
}

bool XBee::readPacket(int timeout) 
{
	if (timeout < 0) 
		return false;

	unsigned long start = millis();

    while (int((millis() - start)) < timeout) 
    {
     	readPacket();

     	if (getResponse().isAvailable()) 
     		return true;

     	else if (getResponse().isError()) 
     		return false;
    }

    return false;
}

void XBee::readPacketUntilAvailable() 
{
	while (!(getResponse().isAvailable() || getResponse().isError()))
		readPacket();
}

void XBee::getResponse(XBeeResponse &response) 
{

	response.setMsbLength(_response.getMsbLength());
	response.setLsbLength(_response.getLsbLength());
	response.setApiId(_response.getApiId());
	response.setFrameLength(_response.getFrameDataLength());
	response.setFrameData(_response.getFrameData());
}

XBeeResponse& XBee::getResponse() 
{
	return _response;
}

void XBee::send(XBeeRequest &request) 
{
	sendByte(START_BYTE, false);

	// send length
	uint8_t msbLen = ((request.getFrameDataLength() + 2) >> 8) & 0xff;
	uint8_t lsbLen = (request.getFrameDataLength() + 2) & 0xff;

	sendByte(msbLen, true);
	sendByte(lsbLen, true);

	// api id
	sendByte(request.getApiId(), true);
	sendByte(request.getFrameId(), true);

	uint8_t checksum = 0;

	// compute checksum, start at api id
	checksum += request.getApiId();
	checksum += request.getFrameId();

	for (int i = 0; i < request.getFrameDataLength(); i++) 
	{
		sendByte(request.getFrameData(i), true);
		checksum+= request.getFrameData(i);
	}

	// perform 2s complement
	checksum = 0xff - checksum;

	// send checksum
	sendByte(checksum, true);

	// send packet (Note: prior to Arduino 1.0 this flushed the incoming buffer, which of course was not so great)
	flush();
}

uint8_t XBee::getNextFrameId() 
{
	_nextFrameId++;

	if (_nextFrameId == 0) // can't send 0 because that disables status response
		_nextFrameId = 1;

	return _nextFrameId;
}

bool XBee::available() 
{
	return _serial->available();
}

uint8_t XBee::read() 
{
	return _serial->read();
} 

void XBee::flush() 
{
	_serial->flush();
} 

void XBee::write(uint8_t val) 
{
	_serial->write(val);
}

void XBee::sendByte(uint8_t messageByte, bool escape) 
{

	if (escape && (messageByte == START_BYTE || messageByte == ESCAPE || messageByte == XON || messageByte == XOFF)) 
	{
		write(ESCAPE);
		write(messageByte ^ 0x20);
	} 
	else 
		write(messageByte);
}

void XBee::resetResponse() 
{
	_pos = 0;
	_escape = false;
	_checksumTotal = 0;
	_response.reset();
}
