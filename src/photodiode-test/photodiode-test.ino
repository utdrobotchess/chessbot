#include <EEPROM.h>
#include <Wire.h>
#include <Photodiode.h>

Photodiode FR_Diode ("FR");
Photodiode FL_Diode ("FL");
Photodiode BR_Diode ("BR");
Photodiode BL_Diode ("BL");

void setup()
{ 
  Serial.begin(57600);
  //Power on the IR LEDS
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
