#include <Arduino.h>
#include <SCoop.h>    // create an instance of the scheduler called mySCoop

defineTask(Task1)    // user quick definition of task1 object
volatile int count;   // force any read/write to memory

void Task1::setup() 
{ 
  count=0; 
}
void Task1::loop()  
{ 
  sleepSync(1000); 
  count++; 
}

defineTask(Task2)    

void Task2::setup() 
{ 
}

void Task2::loop()  
{ 
  digitalWrite(13, HIGH); 
  sleep(100); 
  digitalWrite(13,LOW); 
  sleep(100); 
}

void setup()   
{ 
  Serial.begin(115200);
  mySCoop.start(); 
}
void loop()   
{ 
  int oldcount=-1; 
  yield();
  if (oldcount!=count) 
  { 
    Serial.print("seconds spent :");
    Serial.println(count);
    oldcount=count; 
  } 
}
