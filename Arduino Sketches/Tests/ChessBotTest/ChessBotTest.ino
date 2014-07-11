#include <Wire.h>
#include <ChessBot.h>

ChessBot Bot = ChessBot();
int sign = -1;
byte corrCommand = 7;



void setup()
{
  Serial.begin(9600);
  Bot.Setup();
  attachInterrupt(Bot.rightWheel.encoderInterruptPinRef, RightHandleEncoderPinAInterrupt, RISING); 
  attachInterrupt(Bot.leftWheel.encoderInterruptPinRef, LeftHandleEncoderPinAInterrupt, RISING);  
}

void loop()
{
  //Test 1: See if the robot moves in a three by three square
    //ThreeByThreeSquareMotionTest();
  
  //Test 2: See if the robot moves diagonally the number of squares that is needed.
    //DiagonalMotionTest();
  
  //Test 3:See if Communication is working.
    Bot.CheckForNextMove();
    
}




//Tests Methods: Use these to test and see if they work


/*void RotateTest()
{
  Bot.xBee.GetMessage(); 
  if (Bot.xBee.data == corrCommand)
  {
    for (int i = 1; i < 5; i++)
    {
      sign = sign * -1;
      Bot.Rotate(i*90*sign);
    }
    
    Bot.xBee.data = 0;
  }
  else;
}*/

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
   Bot.Center(90,-90);
}

void DiagonalMotionTest()
{
    Bot.Rotate(45);
    Bot.CrossSquares(2);
    Bot.Rotate(-225);
    Bot.Center(90,-90);
}

void RightHandleEncoderPinAInterrupt()
{
    Bot.rightWheel.HandleEncoderPinAInterrupt();
}


void LeftHandleEncoderPinAInterrupt()
{
    Bot.leftWheel.HandleEncoderPinAInterrupt();
}

