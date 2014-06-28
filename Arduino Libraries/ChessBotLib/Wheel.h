#ifndef Wheel_h
#define Wheel_h

#include <Arduino.h>
#include <digitalWriteFast.h>

class Wheel
{    
	/*	Interrupt	*/
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
		
	public:
    Wheel();
    explicit Wheel(char whichWheel);
	 
    void Rotate(int PWM);
    void HardStop();
    void HandleEncoderPinAInterrupt();
		
    int TICKS_PER_REV;
		
    byte motorPin1;
    byte motorPin2;
    byte encoderInterruptPinRef;
	
    char wheel;
    volatile bool encoderPinBState;
    volatile long encoderTickCount;
    int currentPWM;
    int encoderPinB;
};

#endif