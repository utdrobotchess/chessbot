#ifndef ChessBot_h
#define ChessBot_h

#include "XBee.h"
#include "Locator.h"
#include "Gyroscope.h"
#include "Photodiode.h"
#include "PIDController.h"
#include "Wheel.h"
#include "EEPROM.h"

#define robotIdEEPROMAddress 1
#define DEFAULT_SQUARE_DISTANCE_IN_ENC_TICKS 2950
#define ROBOT_DIAMETER_IN_METERS 0.26
#define MAXIMUM_COMMAND_BUFFER_SIZE 20

class ChessBot
{
public:
    ChessBot();
    void Setup();

    void CheckForNextCommand();
    void ExecuteCommand();
    void SendBotIDCommand();
    void MoveToCommand(uint8_t squareLocation);
    void RCCommand(uint8_t, uint8_t, uint8_t, uint8_t);
    void SendBotLocationCommand();

    uint8_t readBotId();
    void writeBotId(uint8_t Id);
    void HardStop();
    void Rotate(float endAngle);
    void Unwind();
    int  MeasureSquareState();

    void CrossSquares(int numOfSquares, bool measureSquareDistance = true);
    void CrossDiagonal(int numOfSquares);
    void CrossAlongEdge(int numOfSquares);
    void CrossStraight(int numOfSquares, bool measureSquareDistance);

    void Center(int firstEdge, int secondEdge);
    void SmartCenter();
    void AlignToEdge(float targetSpeed = 0.4);
    void MoveDistance(long numOfEncoderTicks, float targetSpeed = 0.4);

    XBee xbee;
    XBeeAddress64 coordinatorAddr64;
    ZBRxResponse rx;

    Wheel leftWheel;
    Wheel rightWheel;

    Gyroscope gyro;

    Locator locator;

    BRPhotodiode backRightPhotoDiode;
    BLPhotodiode backLeftPhotoDiode;
    FRPhotodiode frontRightPhotoDiode;
    FLPhotodiode frontLeftPhotoDiode;

    double angleState;

private:
    long squareDistance;

};

#endif
