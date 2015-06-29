#include <Wire.h>
#include "Wheel.h"

LeftWheel leftWheel;
RightWheel rightWheel;

long startTime = millis();
long time;

void setup()
{
    attachInterrupt(leftWheel.ReturnEncoderInterruptPinRef(), LeftHandleEncoderPinAInterrupt, RISING);
    attachInterrupt(rightWheel.ReturnEncoderInterruptPinRef(), RightHandleEncoderPinAInterrupt, RISING);
    Serial.begin(57600);
}

void loop()
{
    AngularVelocityTest();
}

void RotateTest()
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
}

void EncoderTest()
{
    Serial.print(leftWheel.ReturnEncoderTickCount());
    Serial.print(" ");
    Serial.println(rightWheel.ReturnEncoderTickCount());
    delay(10);
}

void AngularVelocityTest()
{
    rightWheel.ControlAngularVelocity(0.4);
    leftWheel.ControlAngularVelocity(0);
    Serial.print(leftWheel.ReturnCurrentPWM());
    Serial.print(" ");
    Serial.println(leftWheel.MeasureAngularVelocity());
}

void AngularVelocityTrackingTest()
{
    time = millis();
    leftWheel.ControlAngularVelocity(time/2000.0);
}

void RightHandleEncoderPinAInterrupt()
{
    rightWheel.HandleEncoderPinAInterrupt();
}

void LeftHandleEncoderPinAInterrupt()
{
    leftWheel.HandleEncoderPinAInterrupt();
}
