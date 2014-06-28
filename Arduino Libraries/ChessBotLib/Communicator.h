#ifndef Communicator_h
#define Communicator_h

#include <Arduino.h>
				
class Communicator
{    
    #define Error1			"Error- Message was not received!"
    #define Ok              "Message was received!"
    
    #define ID					1
    #define CHECK_SUM			5
		
    #define PACKET_SIZE			3
		
		
    #define ID_LOCATION			0
    #define COMMAND_LOCATION	1
    #define CHECKSUM_LOCATION	2
	public:
    
    Communicator();
    void GetMessage();
    void SetMessageType(byte type);
    void SendMessage(byte message);
    void ExecuteCommand(byte command);

    byte command;
		
	private:
    byte id;
    byte idLocation;
    byte packetSize;
    byte checkSum;
    byte checkSumLocation;
    byte commandLocation;
};

#endif