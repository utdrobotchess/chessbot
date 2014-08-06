#ifndef SCOOP_H
#define SCOOP_H

/******** PREPROCESING CONDITIONS ********/

// SCOOPTRACE enables using trace("x") function in the user program, or even tracing the scheduler behavior, with following values:
// 1=enable trace functions for user only. 
// 2=enable the library to print traces when running mySCoop.start() and mySCoop.yield()
// 3=enable the library to print traces when starting and launching SCoopEvent (task and timers also use basis event object)
// 4=enable the library to print traces when starting tasks or timers 
#define  SCOOPTRACE        1     

// SCOOPTIMEREPORT enable time control variables in SCoopTask and enables cycletime average calculation. accept following values:
// 4 -> 16 cycle average, 3->8 average cycles, 2 ->4 cycle average , 1 -> 2 cycles average, 
// 0 : NO TIME MEASUREMENT , NO VARIABLES yieldMicros,cycleMicros,maxCycleMicros,maxYieldMicros...
#define  SCOOPTIMEREPORT    1 // default value in order to prioritize performance for the user program.

#define  SCOOPOVERLOADYIELD 1 // set to 1 to provides a yield() global function which will overload standard arduino yield()

#define  SCOOPQUANTUMAUTO   0 // set to 1 will provide a mechanisms to adjust task quantum dynamicall, to maintain "constant" cycle time defined by start(x)


#if (ARDUINO >= 102)
#include <Arduino.h>
#else
//#include <WProgram.h>
#error "TESTED ONLY ON 1.0.2"     // delet or hide this line if you still want to try v1.0
#endif

#include "utility/SCoopswitch.h" // rough asm bits and bytes

#if SCOOPTRACE > 0
#include "utility/SCoopdebug.h" // includes the very basic definition library to simplify Serial.print syntax and replace this by "SCp..." types
#endif

#if defined (__AVR__)

#define SCoopDefaultQuantum 400 // recomended before switching to next task. this provide a 5% overhead time used by scheduler, for 3 tasks+loop
#define ptrInt       uint16_t   // used to typecast pointers to integer 
#define millis_t     int16_t    // preferably use 16bits integer on AVR platform... used for low level time handling. could be changed to int32 if needed
#define SCoop_AVR 1             // inform the lbrayr that the code is made for AVR

#elif defined(__MK20DX128__)  || defined (__SAM3X8E__)     // below code enables compiling on ARM 
#define SCoopDefaultQuantum 200; // recomended before switching to next task. this provide a 1% overhead time used by scheduler, for 3 tasks+loop
#define ptrInt       uint32_t    // used to typecast pointers to integer
#define millis_t     int32_t     // we can use long integrer with this 32bit MCU. but this takes more place in RAM ...
#define SCoop_ARM 1              // inform the lbrary that the code is made for ARM

#else
#error "this library might not be compatible with your NON-AVR / ARM platform"
#endif

#define SCoopMillis()   (millis_t)millis()   // overloading the standard millis(), so that this could be optimized later
#define SCoopMicros()   (millis_t)micros()   // overloading the standard micros(), so that this could be optimized later

#define millis32_t      int32_t              // used when we absolutely want to force 32 bit version in sleep or sleepSync
#define SCoopMillis32() (millis32_t)millis()  
#define SCoopMicros32() (millis32_t)micros()  

/********* type defs  *******/

typedef enum itemState_t       // compatible with java Thread
{ itemCONSTRUCTED,             // constructor called but task environement not ready
  itemNEW,                     // a bit of an interim status , before mySCoop.start() is launched
  itemRUNNABLE,                // start() and user setup() already launched
  itemRUNNING,                 // inside the run() or loop()
  itemWAITING,                 // not really working, inside sleep or sleepSync or sleepUntil, or in pause
  itemTERMINATED } itemState_t;// not anymore in the list of item. should not be used anymore

typedef void (*SCoopFunc_t)(); // type definition for a pointer to a function

typedef volatile int8_t   vi8; // hope everyone like it
typedef volatile int16_t  vi16;
typedef volatile int32_t  vi32;
typedef volatile uint8_t  vui8;
typedef volatile uint16_t vui16;
typedef volatile uint32_t vui32;
typedef volatile uint64_t vui64;// yep you can also play with 64 bits variable on this platform without pain
typedef volatile int64_t  vi64;
typedef volatile boolean  vbool;

#define SCoopEventType  1      // used to provide a statical type information to the object in the list (polymorph)
#define SCoopTaskType   2      // not used by the library code , virtual call were prefered even if they are more cpu consuming
#define SCoopTimerType  3          

/********* Objects Prototypes *******/

class SCoopEvent;
class SCoopTask;
class SCoop;

/********* GLOBAL VARIABLE *******/

extern SCoopEvent *  SCoopFirstItem;      // point on the latest registered item in the scheduler list
extern uint8_t       SCoopAtomic;         // will not switch to next task if this variable is not null
extern SCoopEvent *  SCoopCurrent;        // point to the current item launched by scheduler (yield)
extern SCoopTask *   SCoopTaskPtr;        // point to "this" if we are runing code inside a task
extern SCoop         mySCoop;             // one single instance of this object predefined in the library

#if SCOOPOVERLOADYIELD == 1
extern void          yield();             // used to overload the Arduino yield "weak"
#endif
 
/********* SCOOPITEM CLASS *******/

class SCoopEvent                      // represent an object in the SCoop list (task, event, msg...)
{ public: 
  SCoopEvent();                       // basic constructor to register the object in the list
  ~SCoopEvent();                      // destructor to remove item from the list
  void unregister();                  // remove the item from the list
  void init();                        // init the object (an extension of constructor actions)
  
#if SCOOPTRACE > 0                    // if we want to trace whats hapen during start&launch
  void traceThis();                   // specifically display the "this" pointer value, and the SP stack 
  void trace(char * xx);              // display this and the xx string
#else
#define traceThis() ; 
#define trace(x)    ;
#endif

  virtual void loop() // this is the call to user function. can be overloaded by derived objects
  { if (loopHook) { loopHook(); } }  // call the user function instead, if not null
    
  virtual void setup() { }           // called by start. should be overloaded by derived object if needed
  
  virtual void run() 
  { loop(); };                      // run() is same as loop() for this basic item

   
  void set(bool val) { trigger = val; }   // this will force the event object to be launched by scheduler in next round
  void set()         { trigger = true; }  
  
  bool operator=(const bool rhs)     // overload operator assignement to make things event simpler
  { trigger=rhs; return trigger; }

  virtual void start();              // used to init the user object by launching its setup()
  virtual void launch() ;            // launch or switch into this item or its derived. called by scheduler only
  virtual void terminate() { };      // called by destructor. nothing to do for this type of item
                                     // could be used by user to "free" memory allocated if using malloc for stack

  itemState_t getState()             // for compatibility with Java Thread library
  { return state; };  
  
  bool    isAlive()                  // for compatibility with Java Thread library
  { return ((state >= itemNEW) & (state < itemTERMINATED)); }

  uint8_t       itemType;            // place holder for recognizing item. could be replaced by c++ typeid()...
  SCoopFunc_t   loopHook;            // pointer to the user function to call
  SCoopEvent *  pNext;               // point to the next object registered in the list
  volatile itemState_t state;        // for compatibility with Java Thread library
  vbool         trigger;             // the event will be run() if true
  vbool         paused;              // the event will not be run() by the launch() if this variable is true

protected:
};                                   // end of class SCoopEvent
    


/********* FACILITATE EVENT DEFINITION *******/
// this creates a derived object inheriting from SCoopEvent, 
// but with only 1 virtual method run() which has to be defined by the user.

#define defineEvent(myevent) \
class myevent : public SCoopEvent \
{public: bool operator=(const bool rhs) { trigger=rhs; return trigger; } \
void run(); } ; myevent myevent ;
	
	
/********* SCOOPTASK CLASS *******/

class SCoopTask : public SCoopEvent   // this derived item is a task, with its own stack environement
{public:

  SCoopTask();                       // basic constructor
  SCoopTask(SCoopStack_t* stack, ptrInt size);
  SCoopTask(SCoopStack_t* stack, ptrInt size, SCoopFunc_t func);

  ~SCoopTask();                      // task destructor. should be used only if stack is made with malloc()

  void init(SCoopStack_t* stack, ptrInt size);
  
#if SCOOPTRACE > 0
  void trace(char * xx);
#else
#define trace(x)  ;
#endif

  virtual void loop()                // this is the call to user function. should be overloaded by a derived objects
  { if (loopHook) { loopHook(); } }

  
  virtual void setup() { };             // called after start. should be overriden by child objects

  void run() { while(1) { loop(); yield(); } }

  virtual void start();                      // initialize stack environement for calling run/loop

  void yield();                              // potentially switch to pNext object, if time quantum reached             
  
  void yield(millis_t quantum);              // potentially switch to pNext object, if time quantum given is reached
        
  void sleep(millis32_t ms);                 // will replace your usual arduino "delay" function
                                             // 32 bit value enables sleeping more than 32 seconds ! more than one week in fact :=)
  void sleepSync(millis32_t ms);             // same as Sleep but delays are not absolute but relative to previous call to SleepSync 

  void sleepUntil(vbool& var);               // just wait for an external variable to become true. variable will then be flaged to false

  ptrInt stackLeft();                        // remaining stack space in this task

  uint8_t *    pStack;                       // always point back and forth to the SP register for this task
  uint8_t *    pStackAddr;                   // keep a copy of the lowest stack adress. mainly used by stackleft()
#if SCOOPTIMEREPORT > 0                      // verifiy if we want to measure timing
  millis_t     yieldMicros;                  // time spent in the task during 1 complete scheduler cycle (average)
  millis_t     cycleMicros;                  // total time (average) for a cycle 
  millis_t     maxYieldMicros;               // maximum average amount of time spent in the task
  millis_t     maxCycleMicros;               // maximum average amount of time spent in a full cycle
#endif
  millis_t     quantumMicros;                // copy of the SCoopQuantum global definition, so the user can overload the value in setup()
  
  private:  
  virtual void launch() ;                    // launch the task from where it was stop, or just launch run/loop or user function the first time
//virtual void terminate();                  // called before removing the item list // not implemented yet
                                             // nothing to do for us as the stack are supposed to be static
  void backToMain();                         // jump back to the main environement from the task, => arduino loop()
    
  void backToTask();                         // jump to the task from the MainEnv

  void sleepMs(millis32_t ms, bool sync);    // intermediate function called by sleep and sleepsync to optimize code size
  
  void yieldForce(millis_t spent);           // give control back to scheduler in order to switch to next task or come bacok in main loop()
  
  millis32_t   timeStamp;                    // used by SleepSync
  bool         envReady;                     // true means the local stack has been initialised with object context
  millis_t     prevMicros;                   // memorize the value of the micros() counter when entering the task

};

/******* MACRO FOR CREATING TASK OBJECTS Easily ******/

// define a stack as a static array , taking care of stack allignement
#define defineStack(x,y) static SCoopStack_t x [ (  y + sizeof(SCoopStack_t) -1)/ sizeof(SCoopStack_t)];

// define a new object class inheriting from the SCoopTask object
#define defineTask_Size( mytask , stacksize ) \
defineStack( mytask##stack , stacksize )      \
class mytask : public SCoopTask               \
{ public: mytask ():SCoopTask(& mytask##stack [0] , stacksize ){ }; \
void setup(); void loop(); } ; mytask mytask ;

#define defineTask_( task ) defineTask_Size( task , SCoopDefaultStackSize )

// this is used to handle multiple optional parameters in macro ...
#define defineTask_X(x,A,B,FUNC, ...)  FUNC  
#define defineTask(...)  defineTask_X(,##__VA_ARGS__, \
        defineTask_Size(__VA_ARGS__),\
        defineTask_(__VA_ARGS__)) 



/********* SCOOPTIMER CLASS *******/


class SCoopTimer : public SCoopEvent
{ public:
  SCoopTimer();                                // constructor
  SCoopTimer(millis32_t period);
  void  init(millis32_t period);

  void setPeriod(millis32_t newPeriod);        // define the period for launching the timer
  millis32_t getPeriod();                      // return the period variable
  void setTimeToRun(millis32_t time);          // set the next launch time to happen in "time" ms
  millis32_t getTimeToRun();                   // return the value corresponding to the time when the timer will be launched
  void schedule(millis32_t time);              // plan the next launch (same as SetTimeToRun in fact
  void schedule(millis32_t time, int32_t count); // same but with a limited number of occurences (count)

protected:
  void start();                                // initialize timer and make it ready for launch
  void launch();                               // launch the run() if time ellapsed and not paused

private:
  millis32_t timePeriod;                       // store the period for further reload
  millis32_t timeNextLaunch;                   // represent the next time when the object will be launch, relatively to SCoopMillis()
  int32_t    counter;                          // by defaut = -1. if >0 then represent the max number of futur occurences
};


/******* MACRO FOR CREATING TIMER OBJECTS Easily ******/
// define an object class inheriting from SCoopTimer
// user has to define the bject run() method only

#define defineTimer_Period(timer,period) \
class timer : public SCoopTimer          \
{public: timer ():SCoopTimer( period ) { }; \
 void run(); } ; timer timer ;

#define defineTimer_(timer) defineTimer_Period(timer,0)

#define defineTimer_X(x,A,B,FUNC, ...)  FUNC  // trick to create macro with optional arguments
#define defineTimer(...)  defineTimer_X(,##__VA_ARGS__, \
        defineTimer_Period(__VA_ARGS__),\
        defineTimer_(__VA_ARGS__)) 


/******* MAIN SCOOP CLASS ******/

class SCoop                            // this is an interim approach. Most likely the mySCoop object will be instanciated or will inherit from SCoopTask in a later revision
{ public:
  SCoop();                             // constructor
  void start();                        // start all registered objects in the list
  void start(millis_t maxCycleTime);   // same as start but will compute task quantum based on provided user expected cycle time. quantum automatically adjusted in each task
  static void yield();                 // can be called from where ever in order to Force the switch to next task
  void cycle();                        // execute a complete cycle (all tasks , all timer, all event before returning)
  static void sleep(millis32_t time);  // quick implementation of a delay() type of function, in case the startdd Arduino delay doesnt contain yield()
};

/*************** SCOOPFIFO CLASS ******************/
// esay way of handling tx rx buffers for bytes, int or long
class SCoopFifo
{public:
  SCoopFifo(void * fifo, const uint8_t itemSize, const uint16_t itemNumber);
  
  uint16_t count();                   // return number of samples available in the buffer

  bool put(void* var);                // store one sample in the buffer. return true if ok, false if buffer is full

  bool putChar(const uint8_t value);

  bool putInt(const uint16_t value);

  bool putLong(const uint32_t value);

  bool get(void* var);               // provide the older item available in the buffer. return true if ok, false if the buffer is empty

  uint8_t  getChar();                // return the next value in the fifo, as an integer depending on the itemsize. it will wait until available!!!
  uint16_t getInt();                 // return the next value in the fifo, as an integer depending on the itemsize. it will wait until available!!!
  uint32_t getLong();                // return the next value in the fifo, as an integer depending on the itemsize. it will wait until available!!!
  
  uint16_t flush();
   
  operator uint16_t() { return count(); }

  void getYield(void* var);          // return an item and potentially wait until it is available. use yield() in the meantime
  

  uint8_t  itemSize;
  uint8_t* volatile ptrIn;
  uint8_t* volatile ptrOut;
  uint8_t* ptrMin;
  uint8_t* ptrMax;
private:
  };

/*************** MACRO TO CREATE FIFO BUFFER and INSTANCIATE OBJECT  ******************/

#define defineFifo(name,type,number) \
type name##type##number [ number ]; \
SCoopFifo name ( name##type##number , sizeof( type ), number );

#endif

