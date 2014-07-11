#include <Wire.h>
#include <Communicator.h>
Communicator xbee = Communicator();

byte botId = 1;
void setup()
{ 
  Serial.begin(9600);
}

void loop() 
{	
  xbee.CheckForCommand(botId);
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

//----------------------------------------------------------------------------------------
  /*bool commandIsValid = false;
  bool confirmationMessageReceived = false;
  unsigned long checkTime = 2000;
  unsigned long startTime;
  byte tempBuffer[8];
		
  xbee.GetMessage(botId);
	
  for(int index = 0; index < 8; index++)
  {
    xbee.outboxMessageBuffer[index] = xbee.inboxMessageBuffer[index];
    tempBuffer[index] = xbee.inboxMessageBuffer[index];
  }		
  xbee.SendMessage(botId);

  startTime = millis();
  while((millis() - startTime) < checkTime)
  {
    xbee.GetMessage(botId);
    if(xbee.inboxMessageBuffer[COMMAND_INDEX] == 0 && xbee.inboxMessageBuffer[ID_INDEX] == botId)
    {  
      commandIsValid = true;
      confirmationMessageReceived = true;
    }
    byte time = (millis() - startTime);
    Serial.write(time);
  }
	
  if(commandIsValid)
  {
    for(int index = 0; index < 8; index++)
      xbee.inboxMessageBuffer[(index)] = tempBuffer[(index)];
  }
*/

