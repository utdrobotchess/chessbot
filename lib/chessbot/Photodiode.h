#ifndef Photodiode_h
#define Photodiode_h

#include <Arduino.h>

class Photodiode
{    
    #define backRightPhotodiodePin A12
    #define backLeftPhotodiodePin A13
    #define frontLeftPhotodiodePin A14
    #define frontRightPhotodiodePin A15
    
public:
    Photodiode();
    explicit Photodiode(String whichPhotodiode);
    int GetAnalogLightMeasurement();
    bool GetDigitalLightMeasurement(int cutoff);
    
private:
    int photodiodePin;
};

#endif