#ifndef Communicator_h
#define Communicator_h

#include <Arduino.h>

class Communicator
{    
#define ID_INDEX			0
#define COMMAND_INDEX		1
#define CHECKSUM_INDEX		7
	
#define MESSAGE_SIZE		8
	
#define CHECK_SUM			0x75
    
	
public:
    Communicator();
    bool GetMessage(byte botId);
    void SendMessage(byte botId);
    
    byte inboxMessageBuffer[MESSAGE_SIZE];
    byte outboxMessageBuffer[MESSAGE_SIZE];
};

#endif
