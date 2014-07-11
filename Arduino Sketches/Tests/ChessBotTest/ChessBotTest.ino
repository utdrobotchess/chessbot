#include <Wire.h>
#include <ChessBot.h>

ChessBot Bot;
int sign = -1;
byte corrCommand = 7;



void setup()
{
  Bot = ChessBot();
  Bot.Setup();
  attachInterrupt(Bot.leftWheel.ReturnEncoderInterruptPinRef(), LeftHandleEncoderPinAInterrupt, RISING);  
  attachInterrupt(Bot.rightWheel.ReturnEncoderInterruptPinRef(), RightHandleEncoderPinAInterrupt, RISING);
}

void loop()
{
  SquareMotionTest(2);
  DiagonalTest(2);
}

void rotateTest()
{
    for (int i = 1; i < 5; i++)
    {
      sign = sign * -1;
      Bot.Rotate(i*45*sign);
      delay(100);
    }
    
}

void SquareMotionTest(int crossNumberofSquares)
{
   Bot.CrossSquares(crossNumberofSquares);
   Bot.Rotate(90);
   Bot.CrossSquares(crossNumberofSquares);
   Bot.Rotate(90);
   Bot.CrossSquares(crossNumberofSquares);
   Bot.Rotate(90);
   Bot.CrossSquares(crossNumberofSquares);
   Bot.Center(180,-90);
}

void DiagonalTest(int crossNumberofSquares)
{
  Bot.Rotate(45);
  Bot.CrossSquares(crossNumberofSquares);
  Bot.Center(-135,-90);
}

void GyroTest()
{
  Bot.gyro.UpdateAngles();
  Serial.println(Bot.gyro.ReturnZAngle());
}

void CheckForNextMove()
{
  Bot.CheckForNextMove();
}

void RightHandleEncoderPinAInterrupt()
{
    Bot.rightWheel.HandleEncoderPinAInterrupt();
}

void LeftHandleEncoderPinAInterrupt()
{
    Bot.leftWheel.HandleEncoderPinAInterrupt();
}

