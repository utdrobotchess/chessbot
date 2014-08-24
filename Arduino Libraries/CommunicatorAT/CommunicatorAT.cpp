#include "Communicator.h"

/*
 ID_INDEX			0
 COMMAND_INDEX		1
 CHECKSUM_INDEX		7
 
 MESSAGE_SIZE		8
 
 CHECK_SUM			0x75
 */

CommunicatorAT::CommunicatorAT()
{
} 

bool CommunicatorAT::GetMessage(byte botId)
{		 
    bool gotMessage = false;
	int messageIndex = 0;
    
    memset(inboxMessageBuffer, 0, sizeof inboxMessageBuffer);
    
	while ((Serial.available() > 0) && (messageIndex < MESSAGE_SIZE))
	{
		inboxMessageBuffer[messageIndex] = Serial.read();
		messageIndex++;
		delay(3);
	}
	
	if(CHECK_SUM == inboxMessageBuffer[CHECKSUM_INDEX] && ((botId == inboxMessageBuffer[ID_INDEX] || 0xFF == inboxMessageBuffer[ID_INDEX])))
        gotMessage = true;
    else
        memset(inboxMessageBuffer, 0, sizeof inboxMessageBuffer);
    
    return gotMessage;
}

void CommunicatorAT::SendMessage(byte botId)
{
	if(CHECK_SUM == outboxMessageBuffer[CHECKSUM_INDEX] && botId == outboxMessageBuffer[ID_INDEX])
	{
		for(int index = 0; index < 8; index++)
			Serial.write(outboxMessageBuffer[index]);
        Serial.write(0x0D);
	}
	
	memset(outboxMessageBuffer, 0, sizeof outboxMessageBuffer);
}


