#include "Communicator.h"


Communicator::Communicator()
{
    Serial.begin(9600);
} 

/*void Communicator :: SetMessageType(byte type)
{
	
	if (type == 'a')
	{
		//packetSize = NULL;
		checkSumLocation = NULL;
		checkSum = NULL;
		commandLocation = NULL;
	}
}*/
/*
		ID					1
		CHECK_SUM			5
		
		PACKET_SIZE			3
		
		
		ID_LOCATION			0
		COMMAND_LOCATION	1
		CHECKSUM_LOCATION	2
*/
void Communicator::GetMessage()
{		
	id = ID; idLocation = ID_LOCATION;		packetSize = PACKET_SIZE; 	
	checkSumLocation = CHECKSUM_LOCATION;	commandLocation = COMMAND_LOCATION;
	checkSum = CHECK_SUM;
	
	int packetElement = 0;
	
	
	byte packet[packetSize];
	memset(packet, 0, sizeof packet);
	
	while (Serial.available() > 0 && packetElement < packetSize)
	{
		packet[packetElement] = Serial.read();
		packetElement++;
		delay(3);
	}
	
	if (checkSum == packet[2])
	{
		if (id == packet[0])
		{
			command = packet[1];
		}
	}
	
}


void Communicator::ExecuteCommand(byte command)
{
}

void Communicator::SendMessage(byte message)
{
	Serial.write(message);
}


