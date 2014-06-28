#include <Wire.h>
#include <Wheel.h>

Wheel leftWheel('L');
Wheel rightWheel('R'); 

void setup()
{ 
  attachInterrupt(rightWheel.encoderInterruptPinRef, RightHandleEncoderPinAInterrupt, RISING); 
  attachInterrupt(leftWheel.encoderInterruptPinRef, LeftHandleEncoderPinAInterrupt, RISING);  
  Serial.begin(9600);
}

void loop() 
{ 
}

void movementTest()
{
  leftWheel.Rotate(100);
  rightWheel.Rotate(100);
  delay(1000);
  leftWheel.Rotate(-100);
  rightWheel.Rotate(-100);
  delay(1000);
  leftWheel.Rotate(1000);
  rightWheel.Rotate(1000);
  delay(1000);
  leftWheel.Rotate(0);
  rightWheel.Rotate(0);
  delay(1000);
  leftWheel.HardStop();
  rightWheel.HardStop();
}

void encoderTest()
{
    Serial.println(rightWheel.encoderTickCount);
    Serial.println(leftWheel.encoderTickCount);
    delay(10);  
}

void RightHandleEncoderPinAInterrupt()
{
    rightWheel.HandleEncoderPinAInterrupt();
}

void LeftHandleEncoderPinAInterrupt()
{
     leftWheel.HandleEncoderPinAInterrupt();
}
