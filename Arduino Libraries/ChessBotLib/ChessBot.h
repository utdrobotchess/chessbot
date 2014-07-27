#ifndef ChessBot_h
#define ChessBot_h

#include <Communicator.h>
#include <Gyroscope.h>
#include <Photodiode.h>
#include <PIDController.h>
#include <Wheel.h>
#include <EEPROM.h>

#define robotIdEEPROMAddress 1
#define DEFAULT_SQUARE_DISTANCE_IN_ENC_TICKS 2950

class ChessBot
{
public:
    ChessBot();
    void Setup();
    
    void CheckForNextMove();
    void ExecuteCommands();
    byte readBotId();
    void writeBotId(byte Id);
    void HardStop();
    void Rotate(float endAngle);
    void Unwind();
    int  MeasureSquareState();
    
    void CrossSquares(int numOfSquares, bool measureSquareDistance = true);
    void CrossDiagonal(int numOfSquares);
    void CrossAlongEdge(int numOfSquares);
    void CrossStraight(int numOfSquares, bool measureSquareDistance);
    
    void Center(float firstRotation, float secondRotation);
    void AlignToEdge();
    void MoveDistance(long numOfEncoderTicks)
    
    Communicator xBee;
    Wheel leftWheel;
    Wheel rightWheel;
    Gyroscope gyro;
    Photodiode backRightPhotoDiode;
    Photodiode backLeftPhotoDiode;
    Photodiode frontRightPhotoDiode;
    Photodiode frontLeftPhotoDiode;
    
private:
    
    byte commandBuffer[10][8];
    double angleState;
    long squareDistance;
    
};

#endif