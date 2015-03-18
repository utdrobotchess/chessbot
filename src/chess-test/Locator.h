#ifndef LOCATOR_H
#define LOCATOR_H

#include <Arduino.h>

class Locator
{
public:
    Locator(){}
    Locator(uint8_t botId)
    {
        if(botId < 16)
            boardPosition = botId;

        else if(botId < 32 && botId > 15)
            boardPosition = 32 + botId;
    }

    uint8_t GetCurrentLocation()
    { return boardPosition; }

    int GetTravelDistance(int endSquare)
    {
        int startX = boardPosition % 8;
        int startY = boardPosition / 8;

        int endX = endSquare % 8;
        int endY = endSquare / 8;

        if(abs(endX - startX) >= abs(endY - startY))
            return abs(endX - startX);

        else
            return abs(endY - startY);
    }

    void UpdateLocation(int _boardPosition)
    { boardPosition = _boardPosition; }

    float ComputeNextAngle(int endSquare, double currentAngle)
    {
        uint8_t startX = boardPosition % 8;
        uint8_t startY = boardPosition / 8;

        uint8_t endX = endSquare % 8;
        uint8_t endY = endSquare / 8;

        float nextAngle = 0;

        if((endX - startX) == 0 && (endY - startY) > 0)
            nextAngle =  -currentAngle;
        else if((endX - startX) == 0 && (endY - startY) < 0)
            nextAngle =  -currentAngle + 180;

        else if((endX - startX) > 0 && (endY - startY) == 0)
            nextAngle =  -currentAngle + 90;
        else if((endX - startX) < 0 && (endY - startY) == 0)
            nextAngle =  -currentAngle - 90;

        else if((endX - startX) > 0 && (endY - startY) > 0)
            nextAngle =  -currentAngle + 45;
        else if((endX - startX) < 0 && (endY - startY) > 0)
            nextAngle =  -currentAngle - 45;

        else if((endX - startX) > 0 && (endY - startY) < 0)
            nextAngle =  -currentAngle + 135;
        else if((endX - startX) < 0 && (endY - startY) < 0)
            nextAngle =  -currentAngle - 135;

        if(nextAngle > 180)
            nextAngle = nextAngle - 360;

        if(nextAngle < -180)
            nextAngle = nextAngle + 360;


        return nextAngle;
    }

private:
    uint8_t boardPosition;
};
#endif

/* ChessBoard Reference
0,	1,	2,	3,	4,	5,	6,	7,
8,	9,	10,	11,	12,	13,	14,	15,
16,	17,	18,	19,	20,	21,	22,	23,
24,	25,	26,	27,	28,	29,	30,	31,
32,	33,	34,	35,	36,	37,	38,	39,
40,	41,	42,	43,	44,	45,	46,	47,
48,	49,	50,	51,	52,	53,	54,	55,
56,	57,	58,	59,	60,	61,	62,	63,
*/
