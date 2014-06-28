#include <Wire.h>
#include <ChessBot.h>

ChessBot Bot;
int sign = -1;
byte corrCommand = 7;



void setup()
{
  Bot = ChessBot();
  Bot.Setup();
  attachInterrupt(Bot.rightWheel.encoderInterruptPinRef, RightHandleEncoderPinAInterrupt, RISING); 
  attachInterrupt(Bot.leftWheel.encoderInterruptPinRef, LeftHandleEncoderPinAInterrupt, RISING);  
}

void loop()
{
}

void rotateTest()
{
  Bot.xBee.GetMessage(); 
  if (Bot.xBee.command == corrCommand)
  {
    for (int i = 1; i < 5; i++)
    {
      sign = sign * -1;
      Bot.Rotate(i*90*sign);
    }
    
    Bot.xBee.command = 0;
  }
  else;
}

void ThreeByThreeSquareMotionTest()
{
   Bot.CrossSquares(2);
   Bot.Rotate(90);
   Bot.CrossSquares(2);
   Bot.Rotate(90);
   Bot.CrossSquares(2);
   Bot.Rotate(90);
   Bot.CrossSquares(2);
   Bot.Rotate(90);
   Bot.Center();
}

/*void UpdateWheelVelocityTest()
{
  Bot.leftWheel.Rotate(-150);
  Bot.leftWheel.UpdateWheelVelocity();
  Serial.println(Bot.leftWheel.wheelVelocity);
}

void WheelSpeedTest()
{
  Bot.ControlWheelVelocities(6000,-4000);
  Serial.print(Bot.leftWheel.currentPWM);
  Serial.print(" ");
  Serial.println(Bot.leftWheel.wheelVelocity); 
}*/

void RightHandleEncoderPinAInterrupt()
{
    Bot.rightWheel.HandleEncoderPinAInterrupt();
}


void LeftHandleEncoderPinAInterrupt()
{
    Bot.leftWheel.HandleEncoderPinAInterrupt();
}

