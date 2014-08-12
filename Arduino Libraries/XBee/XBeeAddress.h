#ifndef XBEE_ADDRESS
#define XBEE_ADDRESS

#include <Arduino.h>

class XBeeAddress
{
public:
	XBeeAddress();
};

class XBeeAddress64 : public XBeeAddress
{
public:
	XBeeAddress64(uint32_t msb, uint32_t lsb);
	XBeeAddress64();
	uint32_t getMsb();
	uint32_t getLsb();
	void setMsb(uint32_t msb);
	void setLsb(uint32_t lsb);
	//bool operator==(XBeeAddress64 addr);
	//bool operator!=(XBeeAddress64 addr);
private:
	uint32_t _msb;
	uint32_t _lsb;
};

//class XBeeAddress16 : public XBeeAddress {
//public:
//	XBeeAddress16(uint16_t addr);
//	XBeeAddress16();
//	uint16_t getAddress();
//	void setAddress(uint16_t addr);
//private:
//	uint16_t _addr;
//};

#endif