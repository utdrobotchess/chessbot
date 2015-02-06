#include <Wire.h>
#include <EEPROM.h>
#include <ChessBot.h>

ChessBot Bot;

void setup()
{
  Bot.Setup();
  attachInterrupt(Bot.leftWheel.ReturnEncoderInterruptPinRef(), LeftHandleEncoderPinAInterrupt, RISING);  
  attachInterrupt(Bot.rightWheel.ReturnEncoderInterruptPinRef(), RightHandleEncoderPinAInterrupt, RISING);
}

void loop()
{
  //SquareMotionTest(7);
  //DiagonalTest(7);
  CheckForNextMoveTest();
  //MoveDistanceTest();
  //rotateTest();
  //CrossAlongEdgeTest(7);
  //ReadRobotIDTest();
}

void rotateTest()
{
  int sign = 1;
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
   Bot.Center(4,-2);
}

void DiagonalTest(int crossNumberofSquares)
{
  Bot.Rotate(45);
  Bot.CrossSquares(crossNumberofSquares);
  Bot.Center(-3,-2);
}

void GyroTest()
{
  Bot.gyro.UpdateAngles();
  Serial.println(Bot.gyro.ReturnZAngle());
}

void CheckForNextMoveTest()
{
  Bot.CheckForNextMove();
  delay(10);
}

void ReadRobotIDTest()
{
  Serial.println(Bot.readBotId());
  delay(500);
}

void RightHandleEncoderPinAInterrupt()
{
  Bot.rightWheel.HandleEncoderPinAInterrupt();
}

void LeftHandleEncoderPinAInterrupt()
{
  Bot.leftWheel.HandleEncoderPinAInterrupt();
}

void MoveDistanceTest()
{
  Bot.MoveDistance(-200);
}

void CrossAlongEdgeTest(int crossNumberOfSquares)
{
  Bot.AlignToEdge();
  Bot.MoveDistance(500);
  Bot.Rotate(90);
  Bot.CrossSquares(crossNumberOfSquares);
  Bot.Rotate(90);
  Bot.MoveDistance(700);
  Bot.Center(0,2);
}

