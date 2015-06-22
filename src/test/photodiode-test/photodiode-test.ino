#include "Photodiode.h"

FRPhotodiode FR_Diode;
FLPhotodiode FL_Diode;
BRPhotodiode BR_Diode;
BLPhotodiode BL_Diode;

void setup()
{
    Serial.begin(57600);
    pinMode(12, OUTPUT);
    digitalWrite(12,HIGH);
}

void loop()
{
    lightMeasurementTest();
}

void lightMeasurementTest()
{
    Serial.print(FR_Diode.GetAnalogLightMeasurement());
    Serial.print(".");
    Serial.print(FL_Diode.GetAnalogLightMeasurement());
    Serial.print(".");
    Serial.print(BR_Diode.GetAnalogLightMeasurement());
    Serial.print(".");
    Serial.println(BL_Diode.GetAnalogLightMeasurement());
    delay(1000);
}
