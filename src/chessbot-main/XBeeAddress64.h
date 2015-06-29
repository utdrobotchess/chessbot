#ifndef XBEE_ADDRESS
#define XBEE_ADDRESS

#include <Arduino.h>

class XBeeAddress64
{

    public:
        XBeeAddress64(){}
        XBeeAddress64(uint32_t msb, uint32_t lsb)
        { _msb = msb; _lsb = lsb; }
        uint32_t getMsb()
        { return _msb; }
        uint32_t getLsb()
        { return _lsb; }
        void setMsb(uint32_t msb)
        { _msb = msb; }
        void setLsb(uint32_t lsb)
        { _lsb = lsb; }

    private:
        uint32_t _msb;
        uint32_t _lsb;
};

#endif
