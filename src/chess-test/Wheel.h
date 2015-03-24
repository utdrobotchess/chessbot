#ifndef Wheel_h
#define Wheel_h

#include <Arduino.h>
#include "PIDController.h"
#include "digitalWriteFast.h"

#define L_Encoder_interrupt 	0	// 0 is a reference to pin 2
#define R_Encoder_interrupt		1	// 1 is a reference to pin 3

#define L_Encoder_A				2
#define L_Encoder_B				4

#define R_Encoder_A				3
#define R_Encoder_B				5

#define R_Motor_1A				9
#define R_Motor_2A				8

#define L_Motor_3A				11
#define L_Motor_4A				10

#define TICKS_PER_REV           3000.0
#define WHEEL_DIAMETER          0.12065 //Meters

class Wheel
{

public:
    Wheel() {}

    Wheel(uint8_t encA, uint8_t encB, uint8_t M1, uint8_t M2, uint8_t encIntRef)
    {
        this->motorPin1 = M1;
        this->motorPin2 = M2;
        this->encoderInterruptPinRef = encIntRef;
        this->encoderPinA = encA;
        this->encoderPinB = encB;

        currentPWM = 0;
        encoderTickCount = 0;
        previousEncoderTickCount = 0;
        timeOfLastUpdate = micros();

        angularVelocityController = PIDController(5, 18.0, 15.0, 10.0, 255.0, -255.0);
		pinMode(encoderPinA, INPUT);
		digitalWrite(encoderPinA, LOW);
		pinMode(encoderPinB, INPUT);
		digitalWrite(encoderPinB, LOW);

		pinMode(motorPin1, OUTPUT);
		pinMode(motorPin2, OUTPUT);
    }

    int ReturnCurrentPWM()
    { return currentPWM; }

    uint8_t ReturnEncoderInterruptPinRef()
    { return encoderInterruptPinRef; }

    long ReturnEncoderTickCount()
    { return encoderTickCount; }

    void ZeroEncoderTickCount()
    { encoderTickCount = 0; }

    void HandleEncoderPinAInterrupt()
    {
        encoderPinBState = digitalReadFast(encoderPinB);
        encoderTickCount -= encoderPinBState ? -1 : +1;
    }

    void Rotate(int PWM)
    {
        if(currentPWM == PWM)
            return;

        if(PWM > 255)
            PWM = 255;
        else if(PWM < -255)
            PWM = -255;

        currentPWM = PWM;

        if(PWM >= 0) //forward
        {
            analogWrite	(motorPin2, PWM);
            digitalWrite(motorPin1, LOW);
        }
        else         //reverse
        {
            analogWrite	(motorPin1, abs(PWM));
            digitalWrite(motorPin2, LOW);
        }
    }

    float MeasureAngularVelocity()
    {
        now = micros();
        float angularVelocity;

        if(now > timeOfLastUpdate)
        {
            float dt = (now - timeOfLastUpdate)/1000000.0;
            angularVelocity = (encoderTickCount - previousEncoderTickCount)/(TICKS_PER_REV*dt);
            timeOfLastUpdate = now;
        }
        else
            timeOfLastUpdate = now;

        previousEncoderTickCount = encoderTickCount;

        return angularVelocity;
    }

    void ControlAngularVelocity(float angularVelocitySetpoint)
    {
        Rotate((int)angularVelocityController.ComputeOutput(MeasureAngularVelocity(), angularVelocitySetpoint));
    }

    void ResetAngularVelocityController()
    { angularVelocityController.ResetMemory(); }

private:
    uint8_t motorPin1;
    uint8_t motorPin2;
    uint8_t encoderInterruptPinRef;
    uint8_t encoderPinA;
    uint8_t encoderPinB;

    unsigned long timeOfLastUpdate, now;
    long previousEncoderTickCount;
    volatile long encoderTickCount;
    volatile bool encoderPinBState;
    int currentPWM;
    PIDController angularVelocityController;

};

class LeftWheel : public Wheel
{
public:
    LeftWheel() : Wheel(L_Encoder_A, L_Encoder_B, L_Motor_3A, L_Motor_4A, L_Encoder_interrupt){}
};

class RightWheel : public Wheel
{
public:
    RightWheel() : Wheel(R_Encoder_A, R_Encoder_B, R_Motor_1A, R_Motor_2A, R_Encoder_interrupt){}
};

#endif
