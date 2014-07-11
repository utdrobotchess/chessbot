#include <Wire.h>
#include <Communicator.h>
Communicator xbee;
byte botId = 1;

void setup()
{ 
  Serial.begin(9600);
}


void loop() 
{
}

void getMessageTest()
{
  xbee.GetMessage(botId);
  
  for(int i = 0; i < 8; i++)
    Serial.write(xbee.inboxMessageBuffer[i]);
  
  delay(1000);	
}

void sendMessageTest()
{
  xbee.GetMessage(botId);
  for(int i= 0; i < 8; i++)
    xbee.outboxMessageBuffer[i]= xbee.inboxMessageBuffer[i];
    
  xbee.SendMessage(botId);
  delay(1000);
}

void CheckForCommandTest()
{
  xbee.CheckForCommand(botId);
}
