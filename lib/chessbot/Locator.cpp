#include "Locator.h"

Locator::Locator(){}

Locator::Locator(byte botId)
{
	if(botId < 16)
        boardPosition = botId;

	else if(botId < 32 && botId > 15)
        boardPosition = 32 + botId;
}

int Locator::GetCurrentLocation()
{
	return boardPosition;
}

int Locator::GetTravelDistance(int endSquare)
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

void Locator::UpdateLocation(int _boardPosition)
{
    boardPosition = _boardPosition;
}

double Locator::ComputeNextAngle(int endSquare, double currentAngle)
{
    int startX = boardPosition % 8;
    int startY = boardPosition / 8;

    int endX = endSquare % 8;
    int endY = endSquare / 8;

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
