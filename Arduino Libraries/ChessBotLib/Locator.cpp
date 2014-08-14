#include "Locator.h"

Locator::Locator(){}

Locator::Locator(byte botId)
{
	if(botId <= 16)
	{
		x = (botId - 1) % 8;
		y = (botId - 1) / 8;
	}
	else
	{
		x = (botId + 31) % 8;
		y = (botId + 31) / 8;
	}
}

double Locator::ComputeNextAngle(int desiredX,int desiredY, double currentAngle)
{
	if((desiredX-x) == 0 && (desiredY-y) > 0)
		return -currentAngle;
	else if((desiredX-x) == 0 && (desiredY-y) < 0)
		return -currentAngle + 180;

	else if((desiredX-x) > 0 && (desiredY-y) == 0)
		return -currentAngle + 90;
	else if((desiredX-x) < 0 && (desiredY-y) == 0)
		return -currentAngle - 90;

	else if((desiredX-x) > 0 && (desiredY-y) > 0)
		return -currentAngle + 45;
	else if((desiredX-x) < 0 && (desiredY-y) > 0)
		return -currentAngle - 45;

	else if((desiredX-x) > 0 && (desiredY-y) < 0)
		return -currentAngle + 135;
	else if((desiredX-x) < 0 && (desiredY-y) < 0)
		return -currentAngle - 135;

	else
		return 0;
}

int Locator::GetTravelDistance(int desiredX, int desiredY)
{
	if(abs(desiredX-x) >= abs(desiredY-y))
		return abs(desiredX-x);
	else
		return abs(desiredY-y);
}

int Locator::GetCurrentXLocation()
{
	return x;
}

int Locator::GetCurrentYLocation()
{
	return y;
}

void Locator::UpdateCurrentLocation(int currentX, int currentY)
{
	x = currentX; y = currentY;
}