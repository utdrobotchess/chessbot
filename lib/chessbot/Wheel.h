#ifndef Wheel_h
#define Wheel_h

#include <Arduino.h>
#include "PIDController.h"
#include "digitalWriteFast.h"

class Wheel
{    
	/*	Interrupts	*/
    #define L_Encoder_interrupt 	0	// 0 is a reference to pin 2
    #define R_Encoder_interrupt		1	// 1 is a reference to pin 3 
    
	/*	Pins	*/
    #define L_Encoder_A				2
    #define L_Encoder_B				4
    
    #define R_Encoder_A				3
    #define R_Encoder_B				5
    
    #define R_Motor_1A				9
    #define R_Motor_2A				8
    
    #define L_Motor_3A				11
    #define L_Motor_4A				10
    
    /* Constants */
    #define TICKS_PER_REV           3000.0
    #define WHEEL_DIAMETER          0.12065 //Meters
    
public:
    Wheel();
    explicit Wheel(char whichWheel);
    int ReturnCurrentPWM();
    byte ReturnEncoderInterruptPinRef();
    long ReturnEncoderTickCount();
    void ZeroEncoderTickCount();
    void HandleEncoderPinAInterrupt();
    void Rotate(int PWM);
    float MeasureAngularVelocity();
    void ControlAngularVelocity(float angularVelocitySetpoint);
    void ResetAngularVelocityController();
    
private:
    byte motorPin1;
    byte motorPin2;
    byte encoderInterruptPinRef;
    byte encoderPinB;
	
    unsigned long timeOfLastUpdate, now; 
    long previousEncoderTickCount;
    volatile long encoderTickCount;
    volatile bool encoderPinBState;
    int currentPWM;
    PIDController angularVelocityController;
    
};

#endif
