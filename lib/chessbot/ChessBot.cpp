#include "ChessBot.h"

ChessBot::ChessBot()
{	
	xbee = XBee();
	coordinatorAddr64 = XBeeAddress64(0x00000000, 0x00000000); //address of the coordinator
	rx = ZBRxResponse();

	locator = Locator(EEPROM.read(robotIdEEPROMAddress));

	gyro = Gyroscope();
		
	leftWheel = Wheel('L');
	rightWheel = Wheel('R');
		
	backRightPhotoDiode	 =  Photodiode("BR");
	backLeftPhotoDiode 	 =  Photodiode("BL");
	frontRightPhotoDiode =  Photodiode("FR");
	frontLeftPhotoDiode  =  Photodiode("FL");
	
	angleState = 0;
	squareDistance = DEFAULT_SQUARE_DISTANCE_IN_ENC_TICKS;
}

void ChessBot::Setup()
{   
	//turns on the IR LEDs
    pinMode(12, OUTPUT);
    digitalWrite(12, HIGH);

	Serial.begin(57600);
	xbee.setSerial(Serial);   
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
		xbee.readPacket();
		if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
		{
			xbee.getResponse().getZBRxResponse(rx);
			if(rx.getData(0) == 0xFF)
				readyToExecute = true;
			
			else if(commandRowIndex == MAXIMUM_COMMAND_BUFFER_SIZE)
				bufferOverflow = true;
			
			else
			{
                uint8_t* _rx = rx.getData();
				for(int columnIndex = 0; columnIndex < rx.getDataLength(); columnIndex++)
					commandBuffer[commandRowIndex][columnIndex] = _rx[columnIndex];
				
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
	
	while(commandRowIndex < MAXIMUM_COMMAND_BUFFER_SIZE)
	{
		switch(commandBuffer[commandRowIndex][0])
		{
			case 0x1:
				CrossSquares(commandBuffer[commandRowIndex][1]);
				break;
				
			case 0x2:
            {
                int degrees = (commandBuffer[commandRowIndex][0] * 45); 

                if(degrees > 180)
                    degrees = degrees - 360;

				Rotate(degrees);
				break;
            }
			case 0x3:   
				Center((int8_t)commandBuffer[commandRowIndex][1], (int8_t)commandBuffer[commandRowIndex][2]);
				break;
				
			case 0x4:
				writeBotId(commandBuffer[commandRowIndex][1]);
				break;
				
			case 0x5:
			{
				uint8_t message[] = { MeasureSquareState() };
				ZBTxRequest tx = ZBTxRequest(coordinatorAddr64, message, sizeof(message));
				xbee.send(tx);
				break;
			}
				
			case 0x6:
				Unwind();
				break;
				
			case 0x7:
				AlignToEdge();
				break;
				
			case 0x8:
				MoveDistance( (int16_t) (commandBuffer[commandRowIndex][1] << 24) |
										(commandBuffer[commandRowIndex][2] << 16) |
										(commandBuffer[commandRowIndex][3] << 8)  |
										(commandBuffer[commandRowIndex][4])		  );
				break;

			case 0x9:
				Rotate(locator.ComputeNextAngle(commandBuffer[commandRowIndex][1], angleState));
				CrossSquares(locator.GetTravelDistance(commandBuffer[commandRowIndex][1]));
				locator.UpdateLocation(commandBuffer[commandRowIndex][1]);
				break;

			case 0xA:
			{
				uint8_t message[] = { readBotId() };
				ZBTxRequest tx = ZBTxRequest(coordinatorAddr64, message, sizeof(message));
				xbee.send(tx);
				break;
			}
            case 0xB:
                RCMode();  
                break;

            case 0xC:
                locator.UpdateLocation(commandBuffer[commandRowIndex][1]);
                break;

            case 0xD:
            {
                uint8_t message[] = { locator.GetCurrentLocation() };
                ZBTxRequest tx = ZBTxRequest(coordinatorAddr64, message, sizeof(message));
                xbee.send(tx);
                break;
            }

			default:
				break;
		}
		commandRowIndex++;
	}
	
	memset(commandBuffer, 0, sizeof commandBuffer);
	
	
}

int ChessBot::MeasureSquareState()
{
	int BR = backRightPhotoDiode.GetDigitalLightMeasurement(800);
	int BL = backLeftPhotoDiode.GetDigitalLightMeasurement(800);
	int FL = frontLeftPhotoDiode.GetDigitalLightMeasurement(800);
	int FR = frontRightPhotoDiode.GetDigitalLightMeasurement(800);
	
	return (8*BR + 4*BL + 2*FL + FR); //returns a single digit Hex Value
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
	
	unsigned long fineTunePeriod = 200;
	unsigned long startTuneTime  = 0;
	unsigned long timeElapsed    = 0;
	bool          fineTuneEnable = 0;
	
	gyro.Reinitialize();
	
	while (timeElapsed < fineTunePeriod) 
	{
		if(gyro.ReturnZAngle() < endAngle)
			headingController.SetOutputLimits(2, 0.05);
		else
			headingController.SetOutputLimits(-0.05, -2);
		
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

void ChessBot::CrossSquares(int numOfSquares, bool measureSquareDistance)
{    
	byte startingSquare = MeasureSquareState();
	if(!((startingSquare == 0xF) || (startingSquare == 0x0) || (startingSquare == 0x6) || (startingSquare == 0x9)))
		return;
		
	if(abs(angleState) == 45 || abs(angleState) == 135 || abs(angleState) == 225 || abs(angleState) == 315)
		CrossDiagonal(numOfSquares);
	
	else if(startingSquare == 0x9 || startingSquare == 0x6)
		CrossAlongEdge(numOfSquares);
	
	else
		CrossStraight(numOfSquares, measureSquareDistance);
}

void ChessBot::CrossDiagonal(int numOfSquares)
{
	PIDController headingController = PIDController(10, 0.015, 1.0/9000.0, 0.001, 1.0, -1.0);
	PIDController botAccelerator = PIDController(20, 0, 0.01, 0, 1.0, 0.0);
	PIDController adjustAngleIntegrator = PIDController(10, 0, 1, 0, 90, -90);
	
	float targetSpeed = 1;
	float crossingSpeed = 0;
	byte startingSquare = MeasureSquareState();
	int numOfCrossings = 0;
	float adjustAngle = 0;
	bool isHalfway = false;
	
	gyro.Reinitialize();
	
	while(numOfCrossings < numOfSquares)
	{
		byte squareState = MeasureSquareState();
		switch (squareState) 
		{
			case 0xE:
			case 0x1:
			case 0x7:
			case 0x8:
				adjustAngle = adjustAngleIntegrator.ComputeOutput(0, 0.5);
				break;
				
			case 0xD:
			case 0x2:
			case 0xB:
			case 0x4:
				adjustAngle = adjustAngleIntegrator.ComputeOutput(0, -0.5);
				break;
				
			case 0xC:
			case 0x3:
				isHalfway = true;
				break;
				
			default:
				if(numOfSquares - numOfCrossings == 1)
					targetSpeed = 0.5;
				
				if((squareState == startingSquare) && (isHalfway))
				{
					numOfCrossings++;
					isHalfway = false;
				}
				break;
		}
		
		gyro.UpdateAngles();
		
		crossingSpeed = botAccelerator.ComputeOutput(crossingSpeed, targetSpeed);
		leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
		rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
	}
	
	targetSpeed = 0.4;
	long leftEncoderTickCountAtEdge = leftWheel.ReturnEncoderTickCount();
	long rightEncoderTickCountAtEdge = rightWheel.ReturnEncoderTickCount();
	
	while((leftWheel.ReturnEncoderTickCount() - leftEncoderTickCountAtEdge) < squareDistance/1.414 - 500 && 
		  (rightWheel.ReturnEncoderTickCount() - rightEncoderTickCountAtEdge) < squareDistance/1.414 - 500)
	{
		crossingSpeed = botAccelerator.ComputeOutput(crossingSpeed,targetSpeed);
		gyro.UpdateAngles();
		
		leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
		rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
	}
	
	HardStop();
	
}

void ChessBot::CrossAlongEdge(int numOfSquares)
{
	PIDController headingController = PIDController(10, 0.015, 1.0/9000.0, 0.001, 1.0, -1.0);
	PIDController botAccelerator = PIDController(10, 0, 0.01, 0, 2.0, 0.0);
	PIDController adjustAngleIntegrator = PIDController(10, 0, 1, 0, 90, -90);
	
	float targetSpeed = 1.7;
	float crossingSpeed = 0;
	byte startingSquare = MeasureSquareState();
	int numOfCrossings = 0;
	float adjustAngle = 0;
	
	gyro.Reinitialize();
	
	while(numOfCrossings < numOfSquares)
	{
		byte squareState = MeasureSquareState();
		switch (squareState) 
		{
			case 0xE:
			case 0x1:
			case 0xB:
			case 0x4:
				adjustAngle = adjustAngleIntegrator.ComputeOutput(0, 1.5);
				break;
				
			case 0xD:
			case 0x2:
			case 0x7:
			case 0x8:
				adjustAngle = adjustAngleIntegrator.ComputeOutput(0,-1.5);
				break;
				
				
			default:
				if(numOfSquares - numOfCrossings < 3)
				{
					targetSpeed = 1;
					if(numOfSquares - numOfCrossings == 1)
						targetSpeed = 0.5;
				}
				

				
				if((startingSquare == 0x6 && squareState == 0x9) || (startingSquare == 0x9 && squareState == 0x6))
				{
					numOfCrossings++;
					startingSquare = squareState;
				}
				break;
		}
		
		gyro.UpdateAngles();
		
		crossingSpeed = botAccelerator.ComputeOutput(crossingSpeed, targetSpeed);
		leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
		rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
	}

	targetSpeed = 0.4;
	long leftEncoderTickCountAtEdge = leftWheel.ReturnEncoderTickCount();
	long rightEncoderTickCountAtEdge = rightWheel.ReturnEncoderTickCount();
	
	while((leftWheel.ReturnEncoderTickCount() - leftEncoderTickCountAtEdge) < squareDistance/2 - 100 && 
		  (rightWheel.ReturnEncoderTickCount() - rightEncoderTickCountAtEdge) < squareDistance/2 - 100)
	{
		crossingSpeed = botAccelerator.ComputeOutput(crossingSpeed, targetSpeed);
		gyro.UpdateAngles();
		
		leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
		rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
	}
	
	HardStop();
	
}

void ChessBot::CrossStraight(int numOfSquares, bool measureSquareDistance)
{
	PIDController headingController = PIDController(10, 0.015, 1.0/9000.0, 0.001, 1.0, -1.0);
	PIDController botAccelerator = PIDController(10, 0, 0.01, 0, 2.0, 0.0);
	PIDController adjustAngleIntegrator = PIDController(10, 0, 1, 0, 90, -90);
	
	float targetSpeed = 1.7;
	float crossingSpeed = 0;
	byte startingSquare = MeasureSquareState();
	int numOfCrossings = 0;
	float adjustAngle = 0;
	
	gyro.Reinitialize();
	
	while(numOfCrossings < numOfSquares)
	{
		byte squareState = MeasureSquareState();
		long startingLeftEncoderTickCount, startingRightEncoderTickCount;
		bool startedMeasuring = false;
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
				
			case 0xC:
			case 0x3:
				if(startedMeasuring && numOfCrossings == 1 && measureSquareDistance)
				{
					long distanceMeasured = ((leftWheel.ReturnEncoderTickCount() - startingLeftEncoderTickCount) + 
											 (rightWheel.ReturnEncoderTickCount() - startingRightEncoderTickCount))/2;
					
					if(abs(distanceMeasured - squareDistance) > 50)
						squareDistance = distanceMeasured;
				}
				else if(numOfCrossings == 0)
				{
					startingLeftEncoderTickCount = leftWheel.ReturnEncoderTickCount();
					startingRightEncoderTickCount = rightWheel.ReturnEncoderTickCount();
					startedMeasuring == true;
				}
				break;
				
			default:
				if(numOfSquares - numOfCrossings < 3)
				{
					targetSpeed = 1;
					if(numOfSquares - numOfCrossings == 1)
						targetSpeed = 0.5;
				}
				
				if((startingSquare == 0x0 && squareState == 0xF) || (startingSquare == 0xF && squareState == 0x0))
				{
					numOfCrossings++;
					startingSquare = squareState;
				}
				break;
		}
		gyro.UpdateAngles();
		
		crossingSpeed = botAccelerator.ComputeOutput(crossingSpeed, targetSpeed);
		leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
		rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
		
	}
	
	targetSpeed = 0.4;
	long leftEncoderTickCountAtEdge = leftWheel.ReturnEncoderTickCount();
	long rightEncoderTickCountAtEdge = rightWheel.ReturnEncoderTickCount();
	
	while((leftWheel.ReturnEncoderTickCount() - leftEncoderTickCountAtEdge) < squareDistance/2 - 100 && 
		  (rightWheel.ReturnEncoderTickCount() - rightEncoderTickCountAtEdge) < squareDistance/2 - 100)
	{
		crossingSpeed = botAccelerator.ComputeOutput(crossingSpeed, targetSpeed);
		gyro.UpdateAngles();
		
		leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
		rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), adjustAngle));
	}
	
	HardStop();
}

void ChessBot::Center(int firstEdge, int secondEdge)
{
	byte squareState = MeasureSquareState();
	if(!(squareState == 0x0 || squareState == 0xF))
		return;
	
	float firstRotation = firstEdge * 45;
	float secondRotation = secondEdge * 45;

    if(firstRotation > 180)
        firstRotation = firstRotation - 360;

    if(secondRotation > 180)
        secondRotation = secondRotation - 360;

	Rotate(firstRotation);
	AlignToEdge();
	MoveDistance(-squareDistance/2);
	Rotate(secondRotation);
	AlignToEdge();
	MoveDistance(-squareDistance/2);
}

void ChessBot::AlignToEdge(float targetSpeed)
{
	PIDController headingController = PIDController(10, 1.0/300.0, 1.0/6000.0, 7.0/30.0, 1.0/3.0, -1.0/3.0);
	PIDController botAccelerator = PIDController(10, 0, 1, 0, targetSpeed, -targetSpeed);
	byte squareState = MeasureSquareState();
	
	gyro.Reinitialize();
	
	while(squareState == 0x0 || squareState == 0xF)
	{	
		squareState = MeasureSquareState();
		gyro.UpdateAngles();
			
		float crossingSpeed = botAccelerator.ComputeOutput(0, 0.01);
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
}
	
void ChessBot::MoveDistance(long numOfEncoderTicks, float targetSpeed)
{
	PIDController headingController = PIDController(10, 1.0/300.0, 1.0/6000.0, 7.0/30.0, 1.0/3.0, -1.0/3.0);
	PIDController botAccelerator = PIDController(10, 0, 1, 0, targetSpeed, -targetSpeed);
	
	gyro.Reinitialize();
	
	leftWheel.ZeroEncoderTickCount();
	rightWheel.ZeroEncoderTickCount();
	
	if(numOfEncoderTicks > 0)
	{
		while((leftWheel.ReturnEncoderTickCount() < numOfEncoderTicks) && (rightWheel.ReturnEncoderTickCount() < numOfEncoderTicks))
		{
			gyro.UpdateAngles();
		
			float crossingSpeed = botAccelerator.ComputeOutput(0, 0.01);
			leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
			rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
		}
	}
	else
	{
		while((leftWheel.ReturnEncoderTickCount() > numOfEncoderTicks) && (rightWheel.ReturnEncoderTickCount() > numOfEncoderTicks))
		{
			gyro.UpdateAngles();
			
			float crossingSpeed = botAccelerator.ComputeOutput(0, -0.01);
			leftWheel.ControlAngularVelocity(crossingSpeed - headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
			rightWheel.ControlAngularVelocity(crossingSpeed + headingController.ComputeOutput(gyro.ReturnZAngle(), 0));
		}
	}
	
	HardStop();
}

void ChessBot::RCMode()
{
	bool endControllerMode = false;
	float leftWheelVelocity = 0;
	float rightWheelVelocity = 0;
	
	enum TurnDirection
	{
		LEFT = 1,
		RIGHT = -1
	} turnDirection;
	
	enum VelocityDirection
	{
		FORWARD = 1,
		REVERSE = -1
	} velocityDirection;
	
	while(!endControllerMode)
	{
		xbee.readPacket();
		if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
		{
			xbee.getResponse().getZBRxResponse(rx);
			if(rx.getData(0) == 0)
				velocityDirection = REVERSE;
			else
				velocityDirection = FORWARD;
			
			if(rx.getData(2) == 0)
				turnDirection = RIGHT;
			else
				turnDirection = LEFT;
			
			leftWheelVelocity = (float)(velocityDirection * rx.getData(1) - turnDirection * rx.getData(3))/(float)(127.5);
			rightWheelVelocity = (float)(velocityDirection * rx.getData(1) + turnDirection * rx.getData(3))/(float)(127.5);
			
			if(rx.getData(5) == 0xFF)
				endControllerMode = true;
		}
		leftWheel.ControlAngularVelocity(leftWheelVelocity);
		rightWheel.ControlAngularVelocity(rightWheelVelocity);
		delay(10);//Need to fix the ControlAngularVelocity() function within Wheel Class. It should only update samples every 10ms. Once
				  //that is fixed, (probably by using state estimation), then this delay can (and should be) removed. 
	}
	
	HardStop();
}
