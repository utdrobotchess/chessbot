#ifndef ChessBot_h
#define ChessBot_h

#include <Communicator.h>
#include <Gyroscope.h>
#include <Photodiode.h>
#include <PIDController.h>
#include <Wheel.h>
#include <EEPROM.h>

#define robotIdEEPROMAddress 1

class ChessBot
{
public:
    ChessBot();
    void Setup();
    
    void CheckForNextMove();
    void ExecuteCommands();
    byte readBotId();
    void writeBotId(byte Id);
    
    void CrossSquares(int numOfSquares);
    void HardStop();
    void Rotate(float endAngle);
    void Unwind();
    int  MeasureSquareState();
    void Center(float firstRotation, float secondRotation);
    void AlignToEdge();
    
    Communicator xBee;
    Wheel leftWheel;
    Wheel rightWheel;
    Gyroscope gyro;
    Photodiode backRightPhotoDiode;
    Photodiode backLeftPhotoDiode;
    Photodiode frontRightPhotoDiode;
    Photodiode frontLeftPhotoDiode;
    
private:
    
    long squareSize //measured in encoder ticks
    byte commandBuffer[10][8];
    double angleState;
    
};

#endif