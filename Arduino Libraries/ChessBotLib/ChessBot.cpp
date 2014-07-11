#include "ChessBot.h"

ChessBot::ChessBot()
{
    
    xBee = Communicator();
    gyro = Gyroscope();
		
    leftWheel = Wheel('L');
    rightWheel = Wheel('R');
		
    backRightPhotoDiode	 =  Photodiode("BR");
    backLeftPhotoDiode 	 =  Photodiode("BL");
    frontRightPhotoDiode =  Photodiode("FR");
    frontLeftPhotoDiode  =  Photodiode("FL");
    
    angleState = 0;
}

void ChessBot::Setup()
{
	//turn on the IR LEDs
    pinMode(12, OUTPUT);
    digitalWrite(12, HIGH);
    
    Serial.begin(9600);
    
    gyro.Init();
}

byte ChessBot::readBotId()
{
    return EEPROM.read(robotIdEEPROMAddress);
}

void ChessBot::writeBotId(byte Id)
{
    EEPROM.write(robotIdEEPROMAddress, Id);
}

void ChessBot::CheckForNextMove()
{
	uint8_t commandRowIndex = 0;
	bool readyToExecute = false;
	bool bufferOverflow = false;
	
	while(!readyToExecute && !bufferOverflow)
	{
		if(xBee.CheckForCommand(readBotId()))
		{
			if(xBee.inboxMessageBuffer[1] == 0xff)
				readyToExecute = true;
            
			else if(commandRowIndex == 10)
				bufferOverflow = true;
            
			else
			{
				for(int columnIndex = 0; columnIndex < sizeof xBee.inboxMessageBuffer; columnIndex++)
					commandBuffer[commandRowIndex][columnIndex] = xBee.inboxMessageBuffer[(columnIndex)];
                
				commandRowIndex++;
			}		
		}
	}
	
	if(readyToExecute)
		ExecuteCommands();
    
	else if(bufferOverflow)
		memset(commandBuffer, 0, sizeof commandBuffer);
}

void ChessBot::ExecuteCommands()
{
	uint8_t commandRowIndex = 0;
	
	while(commandRowIndex < 10)
	{
		switch(commandBuffer[commandRowIndex][1])
		{
			case 1:
				CrossSquares(commandBuffer[commandRowIndex][2]);
				break;
				
			case 2:
				if (commandBuffer[commandRowIndex][3] == 0x2D)
					Rotate(-(commandBuffer[commandRowIndex][2]));
				else
					Rotate(commandBuffer[commandRowIndex][2]);
				break;
				
			case 3:
				if (commandBuffer[commandRowIndex][4] == 0x2D)
					Center(-(commandBuffer[commandRowIndex][2]),commandBuffer[commandRowIndex][3]);
                
				else if(commandBuffer[commandRowIndex][5] == 0x2D)
					Center(commandBuffer[commandRowIndex][2],-(commandBuffer[commandRowIndex][3]));
                
				break;
				
			default:
				break;
		}
		commandRowIndex++;
	}
}

int ChessBot::MeasureSquareState()
{
	int BR = backRightPhotoDiode.GetDigitalLightMeasurement(800);
	int BL = backLeftPhotoDiode.GetDigitalLightMeasurement(800);
	int FL = frontLeftPhotoDiode.GetDigitalLightMeasurement(800);
	int FR = frontRightPhotoDiode.GetDigitalLightMeasurement(800);
    
	return (8*BR + 4*BL + 2*FL + FR);
}

void ChessBot::HardStop()
{
    leftWheel.Rotate(-leftWheel.ReturnCurrentPWM());
    rightWheel.Rotate(-rightWheel.ReturnCurrentPWM());
    delay(50);
    
    leftWheel.Rotate(0);
    rightWheel.Rotate(0);
    
    leftWheel.ZeroEncoderTickCount();
    rightWheel.ZeroEncoderTickCount();
    leftWheel.ResetAngularVelocityController();
    rightWheel.ResetAngularVelocityController();
}

void ChessBot::Rotate(float endAngle)
{
	PIDController headingController = PIDController(10, 0.003, 1.0/7000.0, 0.01, 2, -2);
	
	unsigned long fineTunePeriod = 50;
	unsigned long startTuneTime  = 0;
	unsigned long timeElapsed    = 0;
	bool          fineTuneEnable = 0;
	
	gyro.Reinitialize();
    
	while (timeElapsed < fineTunePeriod) 
	{
        if(gyro.ReturnZAngle() < endAngle)
            headingController.SetOutputLimits(2, 0.1);
        else
            headingController.SetOutputLimits(-0.1, -2);
        
		if (abs(gyro.ReturnZAngle() - endAngle) < 1)
		{
            if(fineTuneEnable == 0)
            {
                startTuneTime = millis();
                fineTuneEnable = 1;
            }
            else
                timeElapsed = millis() - startTuneTime;
		}
        gyro.UpdateAngles();
        
        leftWheel.ControlAngularVelocity(-headingController.ComputeOutput(gyro.ReturnZAngle(), endAngle));
        rightWheel.ControlAngularVelocity(headingController.ComputeOutput(gyro.ReturnZAngle(), endAngle));
	}
    HardStop();
    
    angleState += endAngle;
    
    if(angleState >= 360)
        angleState += -360;
    else if(angleState <= -360)
        angleState += 360;
}

void ChessBot::Unwind()
{
    Rotate(-angleState);
}

void ChessBot::CrossSquares(int numOfSquares){
    
    PIDController headingController = PIDController(10, 0.015, 1.0/9000.0, 0.001, 1.0, -1.0);
    PIDController botAccelerator = PIDController(20, 0, 1, 0, 1.0, 0.0);
    PIDController adjustAngleIntegrator = PIDController(10, 0, 1, 0, 90, -90);
    
    float crossingSpeed;
    byte startingSquare = MeasureSquareState();
    int numOfCrossings = 0;
    float adjustAngle = 0;
    bool isHalfway = false;
    
    if(!((startingSquare == 0xF) || (startingSquare == 0x0)))
        return;
    
    gyro.Reinitialize();
        
    if(abs(angleState) == 45 || abs(angleState) == 135 || abs(angleState) == 225 || abs(angleState) == 315)
    {
        while(numOfCrossings < numOfSquares)
        {
            byte squareState = MeasureSquareState();
            switch (squareState) 
            {
                case 0xE:
                case 0x1:
                case 0x7:
                case 0x8:
                    adjustAngle = adjustAngleIntegrator.ComputeOutput(0,1.0);
                    break;
                    
                case 0xD:
                case 0x2:
                case 0xB:
                case 0x4:
                    adjustAngle = adjustAngleIntegrator.ComputeOutput(0,-1.0);
                    break;
                    
                case 0xC:
                case 0x3:
                    isHalfway = true;
                    break;
                    
                default:
                    if((squareState == startingSquare) && (isHalfway))
                    {
                        numOfCrossings++;
                        isHalfway = false;
                    }
                    break;
            }
            
            gyro.UpdateAngles();
            
            crossingSpeed = botAccelerator.ComputeOutput(0, 1.0/50.0);
            leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
            rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
        }
    }
    else
    {
        while(numOfCrossings < numOfSquares)
        {
            byte squareState = MeasureSquareState();
            switch (squareState) 
            {
                case 0xD:
                case 0x2:
                case 0x7:
                case 0x8:
                    adjustAngle = adjustAngleIntegrator.ComputeOutput(0,1.5);
                    break;
                    
                case 0xE:
                case 0x1:
                case 0xB:
                case 0x4:
                    adjustAngle = adjustAngleIntegrator.ComputeOutput(0,-1.5);
                    break;
                    
                default:
                    if((startingSquare == 0x0 && squareState == 0xF) || (startingSquare == 0xF && squareState == 0x0))
                    {
                        numOfCrossings++;
                        startingSquare = squareState;
                    }
                    break;
            }
            gyro.UpdateAngles();
            
            crossingSpeed = botAccelerator.ComputeOutput(0, 1.0/50.0);
            leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
            rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
        }
    }

    long leftEncoderTickCountAtEdge = leftWheel.ReturnEncoderTickCount();
    long rightEncoderTickCountAtEdge = rightWheel.ReturnEncoderTickCount();
    
    while( (leftWheel.ReturnEncoderTickCount() - leftEncoderTickCountAtEdge) < 1200 
          && (rightWheel.ReturnEncoderTickCount() - rightEncoderTickCountAtEdge) < 1200 )
    {
        gyro.UpdateAngles();
        
        leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
        rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
    }
    
    HardStop();
}

void ChessBot::Center(float firstRotation, float secondRotation)
{
	byte squareState = MeasureSquareState();
    if(!(squareState == 0x0 || squareState == 0xF))
        return;
    
    Rotate(firstRotation);
    AlignToEdge();
    Rotate(secondRotation);
    AlignToEdge();
}

void ChessBot::AlignToEdge()
{
    PIDController headingController = PIDController(10, 1.0/300.0, 1.0/6000.0, 7.0/30.0, 1.0/3.0, -1.0/3.0);
    PIDController botAccelerator = PIDController(10, 0, 1, 0, 0.4, -0.4);
    byte squareState = MeasureSquareState();
    
    gyro.Reinitialize();
    
    while(squareState == 0x0 || squareState == 0xF)
    {	
        squareState = MeasureSquareState();
        gyro.UpdateAngles();
            
        float crossingSpeed = botAccelerator.ComputeOutput(0, 1.0/100.0);
        leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
        rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
    }
	
	HardStop();
    
	while(!(squareState == 0x3 || squareState == 0xC))
	{	
		squareState = MeasureSquareState();
		if(squareState == 0x2 || squareState == 0xD)
            rightWheel.ControlAngularVelocity(0.4);
        
		else if(squareState == 0x1 || squareState == 0xE)
            leftWheel.ControlAngularVelocity(0.4);
	}
	
	HardStop();
    botAccelerator.ResetMemory();
    
    gyro.Reinitialize();
	
	while((leftWheel.ReturnEncoderTickCount() > -1400) && (rightWheel.ReturnEncoderTickCount() > -1400))
    {
        gyro.UpdateAngles();
        
        float crossingSpeed = botAccelerator.ComputeOutput(0, -1.0/100.0);
        leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
        rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
    }
	
	HardStop();
}
