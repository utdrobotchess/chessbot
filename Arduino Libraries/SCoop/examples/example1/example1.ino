/*** EXAMPLE 2 ***/
// a very expensive led blinking example using most of SCoop objects, as a tuto

#include "Arduino.h"
#include "TimerUp.h"
#include "TimerDown.h"
#include "IOFilter.h"
#include "SCoop.h"

Output led(LED_BUILTIN);                 // easy declaration of our led

TimerDown T1( 0,1000);                   // from x to 0 every 1000ms
TimerUp   T2(TimerUpMillionSec, 1000);   // from 0 to 1 million, every 1000ms
TimerUp   T3(TimerUpBillionMs);          // from 0 to 1 billion every ms

  vbool askOff=false;

  defineEvent(LedOn)
  void LedOn::run() { T3.resume(); led = HIGH;  askOff=true; }

  defineEvent(LedOff)
  void LedOff::run() { T3.pause(); led = LOW;  trace("inside Event ledOff"); }

  vbool tic5Second=false;
  
  defineTimer(sec,5000)
  void sec::run() { trace("inside timer 5sec"); tic5Second=true; LedOn=true; } // set the event ledOn
  
  defineFifo(fifo,uint16_t,10) // use to pass waiting time parameters to the second task

  vbool waitFinished=false;

  defineTask(taskLed)
  void taskLed::setup(){ }
  void taskLed::loop() 
  { trace("taskLed loop");
    sleepUntil(askOff);       // wait the boolean askOff to become true
    fifo.putInt(499);         // push in the fifo.
    fifo.putInt(501);         // push in the fifo.
    sleepUntil(waitFinished); // wait for the second task
    LedOff.set(); }           // set the event called LeddOff so we can expect the led to become LOW  


  defineTask(waiting) 
  void waiting::setup() { }
  void waiting::loop()  {
    trace("waiting loop");
    while (fifo==0) yield(0); // wait fo something to arrive in the fifo
    uint16_t time;
    while (fifo) {
      fifo.get(&time);        // could use time = fifo.getInt() instead
      sleep(time); }
    waitFinished=true;
  }

  char cc;

  void setup()
  { T3.pause(); T3.reset();  // with this sequence we are sure that T3 is not counting and is equal to 0
  
    SCbegin(57600);

    mySCoop.start();     // start the scheduler with default quantum time and launch the setup task

    SCpln("press a key to display its code");
  }
  

  void loop()
  {      
    
//   mySCoop.yield();    // switch to next elligible task or event or timer
   mySCoop.cycle();      // proceed with calling once all tasks and timer and event
    
     if (T1==0) { T1=10; // wait 10 seconds
         SCp("stack left for taskLed = "); SCpln(taskLed.stackLeft()); 
         SCp("stack left for waiting = "); SCpln(waiting.stackLeft()); 
         SCp("led has been HIGH for  = "); SCp(T3);SCpln("ms");
       }

      if (Serial.available()) {
         cc = Serial.read();
         SCp("at T2 = ");SCp(T2);SCp("second, you pressed the key code : ");SCpln((int)cc);
      }
  }
