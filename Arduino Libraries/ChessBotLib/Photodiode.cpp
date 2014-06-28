#include "Photodiode.h"

Photodiode::Photodiode()
{
}

Photodiode::Photodiode(String whichPhotodiode)
{
    
	
	
    if(whichPhotodiode == "FR")
        photodiodePin = frontRightPhotodiodePin;
    
    else if(whichPhotodiode == "FL")
        photodiodePin = frontLeftPhotodiodePin;
    
    else if(whichPhotodiode == "BL")
        photodiodePin = backLeftPhotodiodePin;
    
    else if(whichPhotodiode == "BR")
        photodiodePin = backRightPhotodiodePin;
}

int Photodiode::GetAnalogLightMeasurement()
{	
    return analogRead(photodiodePin);
}

bool Photodiode::GetDigitalLightMeasurement(int cutoff)
{
    return (analogRead(photodiodePin) > cutoff);
}