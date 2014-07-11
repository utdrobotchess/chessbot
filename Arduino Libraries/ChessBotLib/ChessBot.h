#ifndef ChessBot_h
#define ChessBot_h

#include <Communicator.h>
#include <Gyroscope.h>
#include <Photodiode.h>
#include <PIDController.h>
#include <Wheel.h>

class ChessBot
{
public:
    ChessBot();
    void Setup();
    
    void CheckForNextMove();
    void ExecuteCommands();
    
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
    
    byte commandBuffer[10][8];
    double angleState;
    uint8_t robotID;
    
};

#endif