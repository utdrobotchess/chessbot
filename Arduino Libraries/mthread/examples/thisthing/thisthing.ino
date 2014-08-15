// -*- mode: c++ -*-

// Arduino-Compatible Multi-Threading Library (mthread)

// Copyright (C) 2010-2012 Jonathan Lamothe <jonathan@jlamothe.net>

// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this program.  If not, see:
// http://www.gnu.org/licenses/

// See license.txt for more details.

// * * *

// This program demonstrates the EventHandler class by creating an
// event which will only evaluate true periodically and then run for a
// while before waiting for the condition to evaluate true again.  The
// program reports what's going on over the serial console for the
// demonstration.

#include <mthread.h>
#include <Wire.h>
#include <Wheel.h>
#include <Photodiode.h>

Photodiode FR_Diode ("FR");
Photodiode FL_Diode ("FL");
Photodiode BR_Diode ("BR");
Photodiode BL_Diode ("BL");
Wheel leftWheel('L');
Wheel rightWheel('R'); 
long startTime = millis();
long time;

#define COUNT 5
#define SLEEP_TIME 1

class MyEvent : public EventHandler
{
public:
    MyEvent();
protected:
    bool condition();
    bool on_event();
private:
    byte state;
};

MyEvent::MyEvent()
{
    state = MeasureSquareState();
}

bool MyEvent::condition()
{
    sleep(SLEEP_TIME);
    if(state != 0x0 && state != 0xf)
    {
        Serial.println("Event not triggered.");
        rightWheel.ControlAngularVelocity(1);
        leftWheel.ControlAngularVelocity(1);
        delay(100);
        rightWheel.ControlAngularVelocity(0);
        leftWheel.ControlAngularVelocity(0);
        return false;
    }
    Serial.println("Event triggered.");
    return true;
}

bool MyEvent::on_event()
{
    sleep(SLEEP_TIME);
    state = MeasureSquareState();
    if( state > 0x0 && state < 0xf)
    {
        Serial.println("Handler running.");
        RotateTest(45);
        rightWheel.ControlAngularVelocity(1);
        leftWheel.ControlAngularVelocity(1);
        delay(100)
        rightWheel.ControlAngularVelocity(0);
        leftWheel.ControlAngularVelocity(0);
        return true;
    }
    Serial.println("Handler completed.");
    return false;
}

void setup()
{
    attachInterrupt(leftWheel.ReturnEncoderInterruptPinRef(), LeftHandleEncoderPinAInterrupt, RISING);  
    attachInterrupt(rightWheel.ReturnEncoderInterruptPinRef(), RightHandleEncoderPinAInterrupt, RISING);
    pinMode(12, OUTPUT);
    digitalWrite(12,HIGH); 
    Serial.begin(9600);
    main_thread_list->add_thread(new MyEvent);
    delay(1000);
}

byte MeasureSquareState()
{
	int BR = BR_Diode.GetDigitalLightMeasurement(800);
	int BL = BL_Diode.GetDigitalLightMeasurement(800);
	int FL = FL_Diode.GetDigitalLightMeasurement(800);
	int FR = FR_Diode.GetDigitalLightMeasurement(800);
    
	return (8*BR + 4*BL + 2*FL + FR);
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
