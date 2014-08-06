//EXAMPLE 3
// footprint memory


#define WITH_SCOOP 1   // put 0 or 1 and compile this sketch to see the difference on the binary sketch size in byte

volatile uint16_t count;

#if WITH_SCOOP == 1

#include "SCoop.h"

defineTask(task1)
void task1::setup() { count=0; }
void task1::loop()  { count++; }

#else

volatile void increment() { count++; } 

#endif

void setup() { 
#if WITH_SCOOP ==1
  mySCoop.start();
#else
  count=0;
#endif
}

void loop() { 
#if WITH_SCOOP == 1
  mySCoop.yield(); 
#else
  increment();
#endif
}

