#include "Communicator.h"

/*
 ID_INDEX			0
 COMMAND_INDEX		1
 CHECKSUM_INDEX		7
 
 PACKET_SIZE		8
 
 CHECK_SUM			0x75
 */

Communicator::Communicator()
{
} 

bool Communicator::GetMessage(byte botId)
{		 
    bool gotMessage = false;
	int messageIndex = 0;
	byte message[MESSAGE_SIZE] = {0};
	memset(inboxMessageBuffer, 0, sizeof inboxMessageBuffer);
	
	while (Serial.available() > 0 && messageIndex < MESSAGE_SIZE)
	{
		message[messageIndex] = Serial.read();
		messageIndex++;
		delay(3);
	}
	
	if(CHECK_SUM == message[CHECKSUM_INDEX] && ((botId == message[ID_INDEX] || 0xFF == message[ID_INDEX])))
	{
		for(int index = 0; index < 8; index++)
			inboxMessageBuffer[index] = message[index];
        
        gotMessage = true;
	}
    
    return gotMessage;
}

void Communicator::SendMessage(byte botId)
{
	if(CHECK_SUM == outboxMessageBuffer[CHECKSUM_INDEX] && botId == outboxMessageBuffer[ID_INDEX])
	{
		for(int index = 0; index < 8; index++)
			Serial.write(outboxMessageBuffer[index]);
        Serial.write(0x0D);
	}
	
	memset(outboxMessageBuffer, 0, sizeof outboxMessageBuffer);
}

bool Communicator::CheckForCommand(byte botId)
{
	bool commandIsValid = false;
	unsigned long checkTime = 10000;
	unsigned long startTime;
	byte tempBuffer[8];
	
	if(GetMessage(botId))
	{
		for(int index = 0; index < 8; index++)
		{
			outboxMessageBuffer[index] = inboxMessageBuffer[index];
			tempBuffer[index] = inboxMessageBuffer[index];
		}
        
		SendMessage(botId);
        
		startTime = millis();
		while(((millis() - startTime) < checkTime) && !commandIsValid)
		{
			if(GetMessage(botId) && inboxMessageBuffer[COMMAND_INDEX] == 0)
            {
                for(int index = 0; index < 8; index++)
                    inboxMessageBuffer[(index)] = tempBuffer[(index)];
                
                commandIsValid = true;
            }
            delay(10);
		}
	}
	
	return commandIsValid;
	
}


