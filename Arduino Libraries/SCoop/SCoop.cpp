/*****************************************************************************/
/* SCOOP LIBRARY / AUTHOR FABRICE OUDERT / GNU GPL V3                        */
/* https://code.google.com/p/arduino-scoop-cooperative-scheduler-arm-avr/    */
/* VERSION 1.1   XMASS PACK                                                  */
/* ENJOY AND USE AT YOUR OWN RISK  :)                                        */
/* SHOULD READ USER GUIDE FIRST (@!@)                                        */
/*****************************************************************************/


#include "SCoop.h"


/********* GLOBAL VARIABLE *******/

SCoopEvent*   SCoopFirstItem=NULL;   // has to be initialized here. hold the list of all task/event/timer...
uint8_t       SCoopAtomic=1;         // will not switch to next task if this variable is not null
SCoopEvent*   SCoopCurrent=NULL;     // point to the current item ("this") launched by scheduler (yield)
SCoopTask *   SCoopTaskPtr=NULL;     // point to "this" if we are runing code inside a task

/********* LOCAL FILE VARIABLE *******/

static uint8_t *   SCoopMainEnv;               // use to store the SP register of the main loop()

#if SCOOPTIMEREPORT > 0
millis_t SCoopExpectedCycleTime=0;             // beta version for automatic cycle time management
millis_t SCoopStartQuantum = SCoopDefaultQuantum;
uint8_t  SCoopNumberTask=0;                   // number of task registered. just used to dynamically calculate SCoopQuantum by mySCoop.start(x)
#endif


/********* COOPITEM METHODS *******/

SCoopEvent::SCoopEvent()
  { pNext=SCoopFirstItem;                      // memorize the latest item registered
    SCoopFirstItem = this;                     // point the latest item to this one
    itemType=SCoopEventType;       
    init(); };


SCoopEvent::~SCoopEvent()                      // destructor : remove item from the list
{ terminate();                                 // launch the terminate method
  state=itemTERMINATED;
  unregister(); }                              // remove from list
  

void SCoopEvent::unregister()                  // remove item from SCoop list (needed for local objects)
{SCoopEvent * ptr = SCoopFirstItem;            // lets try first one
  if (ptr==this) SCoopFirstItem = ptr->pNext;  // if this item is the last one registered
  else 
  do { if (ptr->pNext==this) {                 // if the next is the one to remove
           ptr->pNext = ptr->pNext->pNext;     // skip it
           break; } }                          // we are done
  while ((ptr=ptr->pNext));                    // try next item until we find the end of the list (NULL)
};

void SCoopEvent::init()                        // called by constructor, after registration
{ trigger = false; 
  paused = false;
  loopHook = NULL;                             // hook call for the loop, if not overriden by a virtual void in child object
  state = itemCONSTRUCTED; };


void SCoopEvent::start() { 
#if SCOOPTRACE > 2
  trace("Event::start");
#endif
  state = itemNEW;                            // very interim state as it is only valid during the first setup()
  paused = false;
  setup();
  state=itemRUNNABLE; };                       


void SCoopEvent::launch() {                   // launch or switch into this item or derived
#if SCOOPTRACE > 2
//  trace("Event::launch");  // removed. too much printing !
#endif  
  if (!paused)
     if (trigger) { trigger=false; 
	   state=itemRUNNING;
	   run(); 
	   state=itemRUNNABLE; } 
  };   


#if SCOOPTRACE > 0
void SCoopEvent::traceThis() { SCoopSei(); // just to make sure we can print something
     SCp("this=");SCphex((ptrInt)this & 0xFFFF);
     int x = (ptrInt)SCoopGetSP();SCp(" SP=");SCphex( x & 0xFFFF); }  // only place where we use SP register, just to see its value
void SCoopEvent::trace(char * xx) { 
     traceThis();SCp(" ");SCpln(xx); }
#endif


/********* SOME BASIC FUNCTIONS *******/

void SCoopMemFill(uint8_t *startp, uint8_t *endp, uint8_t v) 
{ if (startp) while (startp < endp) *startp++ = v; };  
    
ptrInt SCoopMemSearch(uint8_t *startp, uint8_t *endp, uint8_t v) 
{ uint8_t *ptr = startp;
  while (ptr < endp) if (*ptr++ != v) break;
  return ((ptrInt)ptr-(ptrInt)startp-1); 
};

/********* COOPTASK METHODS *******/

// CONSTRUCTORS

SCoopTask::SCoopTask() : SCoopEvent()
{ 
#if SCOOPTIMEREPORT > 0
    SCoopNumberTask++;
#endif
init( NULL,sizeof(SCoopStack_t)); }


SCoopTask::SCoopTask(SCoopStack_t* stack, ptrInt size) : SCoopEvent()
{ 
#if SCOOPTIMEREPORT > 0
    SCoopNumberTask++;
#endif
init(stack,size); }     


SCoopTask::SCoopTask(SCoopStack_t* stack, ptrInt size, SCoopFunc_t func) : SCoopEvent() // this constructor enable user defined context
  { 
#if SCOOPTIMEREPORT > 0
    SCoopNumberTask++;
#endif
init(stack,size); loopHook = func; }     


void SCoopTask::init(SCoopStack_t* stack, ptrInt size)
  { envReady=false; 
    pStackAddr = (uint8_t*)stack; 
    pStack = ((uint8_t*)stack+size-sizeof(SCoopStack_t) );   // prepare task stack to the top of the space provided
    SCoopMemFill(pStackAddr,pStack,0x55);                    // fill with 0x55 patern in order to calculate StackLeft later
//  quantumMicros = SCoopDefaultQuantum;                     // initialized in start()
#if SCOOPTIMEREPORT > 0
    maxYieldMicros = 0;
    maxCycleMicros = 0;
#endif    
    state=itemCONSTRUCTED;
    itemType = SCoopTaskType; };


SCoopTask::~SCoopTask() {        // SCoopItem destructor should be called automatically, for deregistering from list
#if SCOOPTIMEREPORT > 0
SCoopNumberTask--; 
#endif    
}              


/********* ONLY USED IF SCOOPDEBUG FILE INCLUDED AND SCOOPTRACE DEFINED *******/

#if SCOOPTRACE > 0
void SCoopTask::trace(char * xx) { 
     SCoopEvent::traceThis();
     SCp(" @Stack=");SCphex((ptrInt)pStackAddr & 0xFFFF); 
     SCp(" pStack=");SCphex((ptrInt)pStack & 0xFFFF);      
     SCp(" ");SCpln(xx); }
#endif

  

void SCoopTask::start() {
#if SCOOPTRACE > 3
  trace("Task::start");
#endif
    if (pStack) {                                  // sanity check if stack has been allocated by user or constructor
      if (!envReady) { SCoopCli();                 // if the task stack environement is not yet set

	  SCoopSwitch(&SCoopMainEnv,&SCoopMainEnv);// simulate switching but with current context : back to same place !                                               
                                                   // quite equivalent to a "setjmp" mechanism
	  if (envReady) { SCoopSei();              // this will be executed only when we comeback here with a CoopSwitch(&pstack...
             state=itemRUNNING;
             prevMicros=SCoopMicros();          // memorize time , to calculate time spent in the task and in the cycle
             run();  //this->loop(); this->yield(); }
           } 
             
       register uint8_t* pSource = SCoopMainEnv + SCoopSizeEnv;   // force the pointer at the begining of the interim env
       while (pSource>SCoopMainEnv)                // we copy the stack context to the pStack space,  
       { *pStack-- = *pSource--; }                 // this includs the previous return adress so we ll endup just below the previous call to scoopswitch above :)

       SCoopSei();                                 // we can restore interrupts as we are finished with critical stack handling
       envReady=true;                              // stack context and return adress are now ready for next launch
       };  
    SCoopEvent::start();                           // call the user setup function (if defined in derived object)
    sleepSync(0); }                                // this will enable imediate user call to sleepSync to work properly  
} // end start()


void SCoopTask::launch() {
#if SCOOPTRACE > 3
//    trace("Task::launch");  // removed, too much printing !
#endif    
    if (envReady) {                              // make sure the task context is setup first (start() should have been called already ...)
       if (!paused) backToTask(); }              // returning to the latest "SCoopSwith" registered
    else start(); };                             // initialize context if not done in the main arduino setup() section
                               

  void SCoopTask::yield()                       // forces this method to be called , with a "ret" at the end
  { register millis_t spent=SCoopMicros()-prevMicros;
    if (spent>=quantumMicros) yieldForce(spent); }  // switch makes sense, lets force it
   
  void SCoopTask::yield(millis_t quantum)
  { register millis_t spent=SCoopMicros()-prevMicros;
    if (spent>=quantum) yieldForce(spent);  };  // switch makes sense
  
  void SCoopTask::yieldForce(millis_t spent)     // check if time quantum spent in task. if yes, then go back to scheduler
  { if (SCoopAtomic) return;                     // do nothing if we are in a atomic section
#if SCOOPTIMEREPORT > 0                          // verifiy if we want to measure timing
    yieldMicros += spent 
    - (yieldMicros >> SCOOPTIMEREPORT);          // this cumulate the time spent in the task over the 2^x last cycles

    if (yieldMicros>maxYieldMicros) maxYieldMicros = yieldMicros; // check max time to capture peak
#endif
backMain:        
    backToMain(); 
    if (paused) { 
#if SCOOPTIMEREPORT > 0                          // verifiy if we want to measure timing
       yieldMicros=0; 
#endif
       goto backMain; } // if another task or the main loop() put us on pause...
};

  void SCoopTask::backToMain()             // jump back to the main environement , = arduino loop()
  { SCoopTaskPtr=NULL;
    SCoopSwitch(&SCoopMainEnv,&pStack); };        // save context and will come back here


  void SCoopTask::backToTask()                    // jump to the task
  { register millis_t tic=SCoopMicros();

#if SCOOPTIMEREPORT > 0                           // verifiy if we want to measure timing
    register millis_t temp= (cycleMicros>> SCOOPTIMEREPORT);
    cycleMicros += (tic-prevMicros) - temp;       // this should be calculated in the scheduler...
    if (cycleMicros>maxCycleMicros) maxCycleMicros=cycleMicros;

#if SCOOPQUANTUMAUTO == 1
    if ((SCoopExpectedCycleTime > quantumMicros) && (quantumMicros > 10) ) { // automatic adjustement of task quantum time to reach expected cycle time
       if (temp > (SCoopExpectedCycleTime+30))      { quantumMicros--; }  
       else if (temp < (SCoopExpectedCycleTime-30)) { quantumMicros++; }   }
#endif

#endif
    prevMicros = tic;                             // memorize micros counter in order to verify cpu time spent in task
    SCoopTaskPtr=this;                            // we always can find a pointer to the current task in which we are running
    SCoopSwitch(&pStack,&SCoopMainEnv); };        // save context and jump to task. will come back here


  void SCoopTask::sleep(millis32_t ms)              // will replace your usual arduino "delay" function
  { sleepMs(ms,false); };

  void SCoopTask::sleepSync(millis32_t ms)           // same as Sleep but delays are not absolute but relative to previous call to SleepSync
  { sleepMs(ms, true); };
  
  void SCoopTask::sleepMs(millis32_t ms, bool sync)  // same as Sleep but delays are not absolute but relative to previous call to SleepSync
  { if (ms) { 
#if SCOOPTRACE > 3
   trace("Task::sleepms");
#endif   
    ms += (sync ? timeStamp : SCoopMillis32());
              state=itemWAITING;
              while ((ms - SCoopMillis32())>0) yield(0); 
              state=itemRUNNING;
              timeStamp=ms;}
    else  timeStamp=SCoopMillis32(); };               // SleepMs(0,...) will (re)initialize the tempo for relative counting
  

  void SCoopTask::sleepUntil(vbool& var)             // just wait for an "external" variable to become true
  { state=itemWAITING; 
#if SCOOPTRACE > 3
   trace("Task::sleepuntil");
#endif   
    while(!var) yield(0);
    var=false; 
	state=itemRUNNING;};

  ptrInt SCoopTask::stackLeft() 
  { if (pStackAddr) {                                // sanity check if stack has been initialized
       return SCoopMemSearch(pStackAddr, pStack, 0x55);}
    else return 0;
  };


/********* SCOOPTIMER METHODS *******/


SCoopTimer::SCoopTimer() : SCoopEvent()
{init(0); };

SCoopTimer::SCoopTimer(millis32_t period) : SCoopEvent()
{init(period); };


void SCoopTimer::init(millis32_t period) {
 timePeriod = period;
 counter=-1; 
 itemType = SCoopTimerType; };


void SCoopTimer::start() { 
#if SCOOPTRACE > 3
   trace("Timer::start");
#endif   
  SCoopEvent::start(); // launch the setup() method if exist in the child. do nothing by default.
  timeNextLaunch = SCoopMillis32()+timePeriod; 
  state=itemRUNNABLE; }


void SCoopTimer::launch() 
{ if ((counter!=0) &&
      (timePeriod>0) && 
	  (!paused) &&
      ((SCoopMillis32()-timeNextLaunch)>=0)) {
         SCoopAtomic++; // avoid yield
         state=itemRUNNING;
#if SCOOPTRACE > 3
//   trace("Timer::launch/run");  // removed too much
#endif   
         run();  // should be overridden by child
         timeNextLaunch += timePeriod;
         if (counter>0) counter--;
         state=itemRUNNABLE;
         SCoopAtomic--;} };

void SCoopTimer::setPeriod(millis32_t newPeriod)
{ timePeriod = newPeriod; };

millis32_t SCoopTimer::getPeriod()    
{ return timePeriod;};

millis32_t SCoopTimer::getTimeToRun() 
{ if ((counter !=0) && (timePeriod>0))
     return (timeNextLaunch - SCoopMillis32());
  else return 0; };

void SCoopTimer::setTimeToRun(millis32_t time)
{ timeNextLaunch = SCoopMillis32()+time; };

void SCoopTimer::schedule(millis32_t time, int32_t count)
{ setTimeToRun((timePeriod = time)); counter = count; };

void SCoopTimer::schedule(millis32_t time)
{ schedule(time,-1); };


/********* SCOOP METHODS *******/
  
SCoop::SCoop() 
  { //SCoopFirstItem=NULL;     // initialized in the global variable.
    //SCoopAtomic = 1;         
    //SCoopCurrent = NULL;     
    //SCoopTaskPtr = NULL;     
    //SCoopNumberTask=0;
  };
  
#if SCOOPTIMEREPORT >0
  // beta code for definining quantum based on user expected cycle time. 
  // task quantum would then be adjusted dynamically in the SCoopTask::yield() if SCOOPQUANTUMAUTO is set to 1
  void SCoop::start(millis_t maxCycleTime)   // start all objects in the list
  { register millis_t temp = 20* (millis_t)(SCoopNumberTask+1); // at least 20us per (task + loop). dont ask why :)
    if (maxCycleTime < temp ) { maxCycleTime = temp; }
    SCoopExpectedCycleTime = maxCycleTime;   // keep in memory for dynamic adjustement
    SCoopStartQuantum = maxCycleTime / SCoopNumberTask; 
#if SCOOPTRACE > 1
    SCp("expected cycle = ");SCp(SCoopExpectedCycleTime);SCp(", number of task = ");SCp(SCoopNumberTask); SCp(", Start quantum=");SCpln(SCoopStartQuantum);
#endif
    start(); }
#endif    
    
  void SCoop::start()          // start all objects in the list
  { SCoopCurrent= SCoopFirstItem;
#if SCOOPTRACE > 1
    SCp("starting scheduler and ");SCp(SCoopNumberTask);SCp(" tasks");
#endif
    while (SCoopCurrent) {                   
      if (SCoopCurrent->itemType==SCoopTaskType) reinterpret_cast<SCoopTask*>(SCoopCurrent)->quantumMicros=SCoopStartQuantum;
      SCoopCurrent->start();                       // start this task (setup())
      SCoopCurrent=SCoopCurrent->pNext; }          // take next ptr
    SCoopAtomic=0;                                 // ready for switchiching task with "yield"
    };


  void SCoop::yield()          // can be called from where ever in order to Force the switch to next task
  { if (SCoopAtomic) return;  
    if (SCoopTaskPtr) SCoopTaskPtr->yield();       // we ve been called from a task context
    else 
      if (SCoopCurrent==NULL) {                    // most likely the first time we go through here
usefirst:
         if ((SCoopCurrent=SCoopFirstItem)==NULL) return; // no items registered in the list yet
         SCoopCurrent->launch();}                  // jump to the first task : from the main loop()
    else{ // not null
       SCoopCurrent = SCoopCurrent->pNext;         // switch to next one
       if (SCoopCurrent) SCoopCurrent->launch(); 
       else goto usefirst; };                      // jump in the new task
  };

void SCoop::cycle()                                // execute a complete cycle across all tasks & events
{ if (SCoopTaskPtr) return;
  if ((SCoopCurrent=SCoopFirstItem)==NULL) return;
#if SCOOPTRACE > 1
    SCpln("start cycle()");
#endif
  do { SCoopCurrent->launch(); } while ((SCoopCurrent = SCoopCurrent->pNext)); }


  void SCoop::sleep(millis32_t time)               // this unfortunately becomes redundant with the SCoopTask::sleep ...
{ millis32_t target = time + SCoopMillis32();      // mostlikey the mySCoop object will be changed to a SCooTask in a later version
  while (target - SCoopMillis32() >0) yield(); }


/********* DEFAULT INSTANTIATION *******/

SCoop mySCoop;                                    // then we can use it in the main sketch directly

#if SCOOPOVERLOADYIELD == 1
void yield() { SCoop::yield(); };                 // overload standard arduino yield
#endif


/*************** FIFO *****************/

SCoopFifo::SCoopFifo(void * fifo, const uint8_t itemSize, const uint16_t itemNumber)
   { this->itemSize = itemSize;
     ptrMin = ptrIn = ptrOut = (uint8_t*)fifo;   
     ptrMax = (uint8_t*)fifo + (itemNumber * itemSize); }

uint16_t SCoopFifo::count() {                 // return the number of item currently in the fifo
     register int16_t temp;
     SCoop_AVR_sreg_register;

	 SCoop_AVR_SREG_CLI(sreg); temp = (ptrIn-ptrOut);  SCoop_AVR_SREG(sreg);
     return (temp<0 ? (temp + ptrMax-ptrMin) : temp); }

bool SCoopFifo::put(void* var)                  // sequences optimized for AVR GCC
   {   register uint8_t N = itemSize;
       register uint8_t* dest;
       SCoop_AVR_sreg_register;

	   SCoop_AVR_SREG_CLI(sreg); dest = ptrIn; SCoop_AVR_SREG(sreg);
       register uint8_t* post = dest + N;
       if (post >= ptrMax) { post = ptrMin; }
       SCoop_AVR_SREG_CLI(sreg);
	   if (post != ptrOut) { // no overload
          SCoop_AVR_SREG(sreg);
		  register uint8_t* source = (uint8_t*)var;        
          do { (*(dest++)) = (*(source++)); } while (--N);
          SCoop_AVR_SREG_CLI(sreg); ptrIn=post; SCoop_AVR_SREG(sreg);
          return true;
      } else { SCoop_AVR_SREG(sreg); return false; } } // buffer full


bool SCoopFifo::putChar(const uint8_t value) {
   uint8_t X=value;  return put(&X); }

bool SCoopFifo::putInt(const uint16_t value) {
   uint16_t X=value;  return put(&X); }

bool SCoopFifo::putLong(const uint32_t value) {
   uint32_t X=value;  return put(&X); }


bool SCoopFifo::get(void* var)
   { register uint8_t* In;
	 register uint8_t* source;
     SCoop_AVR_sreg_register;

     SCoop_AVR_SREG_CLI(sreg); In=ptrIn; source=ptrOut; SCoop_AVR_SREG(sreg);
     if (In != source) {
       register uint8_t N = itemSize;
       register uint8_t* dest = (uint8_t*)var;      
       do { (*(dest++)) = (*(source++)); } while (--N) ;
       if (source >= ptrMax) { source = ptrMin; }
       SCoop_AVR_SREG_CLI(sreg); ptrOut = source; SCoop_AVR_SREG(sreg);
       return true;
     } else return false; }  // buffer empty

void SCoopFifo::getYield(void* var)
{ register uint8_t* In;
  register uint8_t* Out;
  SCoop_AVR_sreg_register;
  
  do { 
     SCoop_AVR_SREG_CLI(sreg); In=ptrIn; Out=ptrOut; SCoop_AVR_SREG(sreg);
     if (In == Out) yield(); 
    } while (In == Out);
	get(var); }


uint8_t SCoopFifo::getChar()
{ uint8_t result8; 
  getYield(&result8); return result8; }

uint16_t SCoopFifo::getInt()
{ uint16_t result16; 
  getYield(&result16); return result16; }

uint32_t SCoopFifo::getLong()
{ uint32_t result32; 
  getYield(&result32); return result32; }


uint16_t SCoopFifo::flush() { 
  SCoopCli();
  ptrIn = ptrOut = ptrMin; 
  SCoopSei();
  return (ptrMax-ptrMin); }

