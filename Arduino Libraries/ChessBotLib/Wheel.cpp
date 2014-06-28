#include "Wheel.h"

/*	
	Interrupt:
		L_Encoder_interrupt 	pin 0	<- 0 is a reference to pin 2 
		R_Encoder_interrupt 	pin 1	<- 1 is a reference to pin 3 
	Pins:		
		L_Encoder_A				pin 2
		L_Encoder_B				pin 4		
		
		R_Encoder_A				pin 3
		R_Encoder_B				pin 5
			
		R_Motor_1A				pin 9
		R_Motor_2A				pin 8
								
		L_Motor_3A				pin 11
		L_Motor_4A				pin 10
*/

Wheel::Wheel()
{
}

Wheel::Wheel(char whichWheel)
{

	TICKS_PER_REV = 3000;
	
	if(whichWheel == 'L')
	{
		pinMode(L_Encoder_A, INPUT);       
		digitalWrite(L_Encoder_A, LOW);  
		pinMode(L_Encoder_B, INPUT);
		digitalWrite(L_Encoder_B, LOW);
		
		pinMode(L_Motor_3A, OUTPUT); 
		pinMode(L_Motor_4A, OUTPUT);
		
		encoderInterruptPinRef = L_Encoder_interrupt;
		encoderPinB = L_Encoder_B;
	
		motorPin1 = L_Motor_3A;
		motorPin2 = L_Motor_4A;
		
		wheel = whichWheel;
	}
	else if(whichWheel == 'R')
	{
		pinMode(R_Encoder_A, INPUT); 
		digitalWrite(R_Encoder_A, LOW); 
		pinMode(R_Encoder_B, INPUT);
		digitalWrite(R_Encoder_B, LOW);
		
		pinMode(R_Motor_1A, OUTPUT);
		pinMode(R_Motor_2A, OUTPUT);
		
		encoderInterruptPinRef = R_Encoder_interrupt;
		encoderPinB = R_Encoder_B;
		
		motorPin1 = R_Motor_1A;
		motorPin2 = R_Motor_2A;

		wheel = whichWheel;
	}	
}

void Wheel::Rotate(int PWM)
{
	 
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

void Wheel::HandleEncoderPinAInterrupt() //might need to make a interrupt class
{
	encoderPinBState = digitalReadFast(encoderPinB);
	if (wheel== 'L')
		encoderTickCount -= encoderPinBState ? -1 : +1; 
	else
		encoderTickCount += encoderPinBState ? -1 : +1;
}

void Wheel::HardStop()
{
	Rotate(-1*currentPWM);
	delay (50);
	Rotate(0);
}
