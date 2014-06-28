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
    void CrossSquares(int numOfSquares);
    void Rotate(double endAngle);
    void UpdateSquareState();
    void Center();
    void AlignToBlackEdge();
    void AlignToWhiteEdge();
    
    Communicator xBee;
    Wheel leftWheel;
    Wheel rightWheel;
    Gyroscope gyro;
    Photodiode backRightPhotoDiode;
    Photodiode backLeftPhotoDiode;
    Photodiode frontRightPhotoDiode;
    Photodiode frontLeftPhotoDiode;
		
    String squareState;
    double angleState;
			
};

#endif