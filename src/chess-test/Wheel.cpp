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
    currentPWM = 0;
	encoderTickCount = 0;
    previousEncoderTickCount = 0;
    timeOfLastUpdate = micros();
    
    angularVelocityController = PIDController(5, 18.0, 15.0, 10.0, 255.0, -255.0);
    
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
        
	}	
}

void Wheel::ZeroEncoderTickCount()
{
    encoderTickCount = 0;
}

long Wheel::ReturnEncoderTickCount()
{
    return encoderTickCount;
}

int Wheel::ReturnCurrentPWM()
{
    return currentPWM;
}

byte Wheel::ReturnEncoderInterruptPinRef()
{
    return encoderInterruptPinRef;
}

void Wheel::HandleEncoderPinAInterrupt()
{
	encoderPinBState = digitalReadFast(encoderPinB);
    encoderTickCount -= encoderPinBState ? -1 : +1; 
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

float Wheel::MeasureAngularVelocity() 
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

void Wheel::ControlAngularVelocity(float angularVelocitySetpoint)
{
    Rotate((int)angularVelocityController.ComputeOutput(MeasureAngularVelocity(), angularVelocitySetpoint));
}

void Wheel::ResetAngularVelocityController()
{
    angularVelocityController.ResetMemory();
}
