#include <Wire.h>
#include <Communicator.h>
Communicator xbee;

void setup()
{ 
  Serial.begin(9600);
}


void loop() 
{
  sendBackCommand();
}

void sendBackCommand()
{
  xbee.GetMessage();
  xbee.SendMessage(xbee.command);
}
