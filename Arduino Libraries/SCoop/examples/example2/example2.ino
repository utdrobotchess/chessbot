// EXAMPLE 1

#include "SCoop.h"

defineTask(myTask1)
void myTask1::setup() { trace("task1setup");   }
void myTask1::loop()  { Serial.println("hello from task1"); sleep(1000); }

defineTask(myTask2)
void myTask2::setup() { trace("task2setup"); pinMode(LED_BUILTIN, OUTPUT); }
void myTask2::loop()  { 
  Serial.println("led HIGH"); digitalWrite(LED_BUILTIN, HIGH); sleep(500);
  Serial.println("led LOW");  digitalWrite(LED_BUILTIN, LOW);  sleep(500); }

void setup() { Serial.begin(57600); while (!Serial); delay(500); mySCoop.start(); }
void loop()  { Serial.println("do whatever you want here also"); mySCoop.sleep(500); }

