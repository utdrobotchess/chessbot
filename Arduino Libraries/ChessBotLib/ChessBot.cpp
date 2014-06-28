#include "ChessBot.h"

ChessBot::ChessBot()
{
    
    xBee = Communicator();
    gyro = Gyroscope();
		
    leftWheel  = Wheel('L');
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
	
    gyro.Init();
}

void ChessBot::CrossSquares(int numOfSquares){
    
    PIDController PID = PIDController(10, 6.0, 0.05, 70, 70, -70);
    PIDController velocityIntegrator = PIDController(10, 0, -2, 0, 0, -80);
    byte lookForCrossingSwitch = 0;
    String startingSquare;
    int numOfCrossings = 0;
    int crossingSpeed = 150;
    float adjustAngle;
    
    UpdateSquareState();
    if((squareState == "f") || (squareState == "0"))
    {
        startingSquare = squareState;
        gyro.Reinitialize();
        
        if(abs(angleState) == 45 || abs(angleState) == 135 || abs(angleState) == 225 || abs(angleState) == 315)
        {
            while(numOfCrossings < numOfSquares)
            {
                UpdateSquareState();
                switch (lookForCrossingSwitch)
                {
                    case 0:
                        if((squareState == "e") || (squareState == "1"))
                        {
                            adjustAngle += 1.5f;
                            break;
                        }
                        if((squareState == "d") || (squareState == "2"))
                        {
                            adjustAngle += -1.5f;
                            break;
                        }
                        
                        if((squareState == "c") || (squareState == "3"))
                            lookForCrossingSwitch = 1;
                        break;
                    case 1:
                        if((squareState == "b") || (squareState == "4"))
                        {
                            adjustAngle += -1.5f;
                            break;
                        }
                        if((squareState == "7") || (squareState == "8"))
                        {
                            adjustAngle += 1.5f;
                            break;
                        }
                        if(startingSquare == squareState)
                        {
                            numOfCrossings++;
                            lookForCrossingSwitch = 0;
                        }
                        break; 
                }
                
                gyro.UpdateAngles();
                leftWheel.Rotate(crossingSpeed - PID.ComputeOutput(gyro.anglesXYZ[2], adjustAngle));
                rightWheel.Rotate(crossingSpeed + PID.ComputeOutput(gyro.anglesXYZ[2], adjustAngle));
                delay(10);
                
            }
        }
        else
        {
            while(numOfCrossings < numOfSquares)
            {
                
                UpdateSquareState();
                switch (lookForCrossingSwitch)
                {
                    case 0:
                        if((squareState == "e") || (squareState == "1"))
                        {
                            adjustAngle += -1.5f;
                            break;
                        }
                        if((squareState == "d") || (squareState == "2"))
                        {
                            adjustAngle += 1.5f;
                            break;
                        }
                        
                        if((squareState == "c") || (squareState == "3"))
                            lookForCrossingSwitch = 1;
                        break;
                    case 1:
                        if((squareState == "b") || (squareState == "4"))
                        {
                            adjustAngle += -1.5f;
                            break;
                        }
                        if((squareState == "7") || (squareState == "8"))
                        {
                            adjustAngle += 1.5f;
                            break;
                        }
                        if((startingSquare == "f" && squareState == "0") || (startingSquare == "0" && squareState == "f"))
                        {
                            numOfCrossings++;
                            lookForCrossingSwitch = 0;
                            startingSquare = squareState;
                        }
                        break;
                }
                
                gyro.UpdateAngles();
                leftWheel.Rotate(crossingSpeed - PID.ComputeOutput(gyro.anglesXYZ[2], adjustAngle));
                rightWheel.Rotate(crossingSpeed + PID.ComputeOutput(gyro.anglesXYZ[2], adjustAngle));
                delay(10);
                
            }
        }
    }
    
    leftWheel.encoderTickCount = 0;
    rightWheel.encoderTickCount = 0;
    
    while((abs(leftWheel.encoderTickCount) < 1000) && (abs(rightWheel.encoderTickCount) < 1000))
    {
        crossingSpeed += -4;
        gyro.UpdateAngles();
        leftWheel.Rotate(crossingSpeed - PID.ComputeOutput(gyro.anglesXYZ[2], adjustAngle));
        rightWheel.Rotate(crossingSpeed + PID.ComputeOutput(gyro.anglesXYZ[2], adjustAngle));
    }
    
    leftWheel.HardStop();
	rightWheel.HardStop();
    
}

void ChessBot::Rotate(double endAngle)
{
	PIDController PID = PIDController(10, 2.0, 0.01, 100, 255, -255);
	
	float fineTunePeriod = 50;
	float startTuneTime  = 0;
	float timeElapsed    = 0;
	bool  fineTuneEnable = 0;
	
	gyro.Reinitialize();
    
	while (timeElapsed < fineTunePeriod) 
	{
        gyro.UpdateAngles();
        
        if(gyro.anglesXYZ[2] < endAngle)
            PID.SetOutputLimits(255, 70);
        else
            PID.SetOutputLimits(-70, -255);

        leftWheel.Rotate(-PID.ComputeOutput(gyro.anglesXYZ[2], endAngle));
		rightWheel.Rotate(PID.ComputeOutput(gyro.anglesXYZ[2], endAngle));
		
		if (abs(gyro.anglesXYZ[2] - endAngle) < 1)
		{
				if(fineTuneEnable == 0)
				{
					startTuneTime= millis();
					fineTuneEnable = 1;
				}
				else
					timeElapsed = millis() - startTuneTime;
		}
	}
	
	leftWheel.HardStop();
	rightWheel.HardStop();
    
    angleState += endAngle;
}

void ChessBot::Center()
{
	UpdateSquareState();
	if (squareState == "0")
	{
		Rotate(90);
        AlignToWhiteEdge();
		Rotate(-90);
        AlignToWhiteEdge();
	}
	else if (squareState == "f")
	{
		Rotate(90);
        AlignToBlackEdge();
		Rotate(-90);
        AlignToBlackEdge();
	}
	else;
}

void ChessBot::UpdateSquareState()
{
	int BR = backRightPhotoDiode.GetDigitalLightMeasurement(800)*8;
	int BL = backLeftPhotoDiode.GetDigitalLightMeasurement(800)*4;
	int FL = frontLeftPhotoDiode.GetDigitalLightMeasurement(800)*2;
	int FR = frontRightPhotoDiode.GetDigitalLightMeasurement(800);
	squareState = String( BR + BL + FL + FR, HEX);
}

void ChessBot::AlignToWhiteEdge()
{
    int speed = 70; 
    PIDController PID = PIDController(10, 8.0, 0.05, 70, 70, -70);
    gyro.Reinitialize();
    
	while(squareState == "0")
	{	
        gyro.UpdateAngles();
		UpdateSquareState();
        leftWheel.Rotate(speed - PID.ComputeOutput(gyro.anglesXYZ[2], 0));
		rightWheel.Rotate(speed + PID.ComputeOutput(gyro.anglesXYZ[2], 0));
	}
	
	leftWheel.HardStop();
	rightWheel.HardStop();
	
	while(squareState != "3")
	{	
		UpdateSquareState();
        
		if(squareState== "2")
		{
			leftWheel.Rotate(0);
			rightWheel.Rotate(90);
		}
		else if(squareState == "1")
		{
			leftWheel.Rotate(90);
			rightWheel.Rotate(0);
		}
	}
	
	leftWheel.HardStop();
	rightWheel.HardStop();
	
	leftWheel.encoderTickCount = 0;
	rightWheel.encoderTickCount = 0;
    
    gyro.Reinitialize();
	
	while((abs(leftWheel.encoderTickCount) < 1400) && (abs(rightWheel.encoderTickCount) < 1400))
    {
        gyro.UpdateAngles();
        leftWheel.Rotate(-speed - PID.ComputeOutput(gyro.anglesXYZ[2], 0));
		rightWheel.Rotate(-speed + PID.ComputeOutput(gyro.anglesXYZ[2], 0));
    }
	
	leftWheel.HardStop();
	rightWheel.HardStop();
	
	UpdateSquareState();
}

void ChessBot::AlignToBlackEdge()
{
    int speed = 70; 
    PIDController PID = PIDController(10, 8.0, 0.05, 70, 70, -70);
    gyro.Reinitialize();
    
	while(squareState == "f")
	{	
        gyro.UpdateAngles();
		UpdateSquareState();
        leftWheel.Rotate(speed - PID.ComputeOutput(gyro.anglesXYZ[2], 0));
		rightWheel.Rotate(speed + PID.ComputeOutput(gyro.anglesXYZ[2], 0));
	}
	
	leftWheel.HardStop();
	rightWheel.HardStop();
	
	while(squareState != "c")
	{	
		UpdateSquareState();
        
		if(squareState== "d")
		{
			leftWheel.Rotate(0);
			rightWheel.Rotate(90);
		}
		else if(squareState == "e")
		{
			leftWheel.Rotate(90);
			rightWheel.Rotate(0);
		}
	}
	
	leftWheel.HardStop();
	rightWheel.HardStop();
	
	leftWheel.encoderTickCount = 0;
	rightWheel.encoderTickCount = 0;
    
    gyro.Reinitialize();
	
	while((abs(leftWheel.encoderTickCount) < 1400) && (abs(rightWheel.encoderTickCount) < 1400))
    {
        gyro.UpdateAngles();
        leftWheel.Rotate(-speed - PID.ComputeOutput(gyro.anglesXYZ[2], 0));
		rightWheel.Rotate(-speed + PID.ComputeOutput(gyro.anglesXYZ[2], 0));
    }
	
	leftWheel.HardStop();
	rightWheel.HardStop();
	
	UpdateSquareState();
}
