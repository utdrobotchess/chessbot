
//*** performance1 ***//
// performance measurement


#include "SCoop.h"

#define led_pin LED_BUILTIN 

vui32 count1=0, count2=0, count3=0, count4=0;;

void justCount32() { 
count3++;count1++;  // each count needs 1,25uS on AVR 16MHZ (maximum 8 millions in 10 seconds then )
count4++;count2++;  // these 32 counts represent a 40us effective CPU work in the loop
count3++;count1++;  // then the call to yield is made at the end of the loop to check against quantum time (500us in this example)
count4++;count2++;  // imediately back to the beginignof the loop if the time spent is not > 500us.
count1++;count4++;  // basically this loop should be executed 10 times every 400us, but the yield test and switch also consume tenth micros seconds
count2++;count3++;  // for this example, the % of CPU "lost" in the yield test and switching is 16.6%, as the total count reach 6 670 000 instead of 8 million max
count3++;count1++;  // but if the effective CPU work is more intensive in the loop (like an fft) then the yield becomes relatively small and the % CPU lost will reduce 
count4++;count2++;  // in this example, we loose 16.6% = 1.66seconds during this time we have checked the yield 250000 times and switched from task1 to task 2 to main loop 20000 times
count1++;count4++;             
count2++;count3++;  // this example is built to demonstrate that a cooperative scheduler can be as efficient as a preemptive os 
count3++;count1++;  // IF and only IF the time spent in the task is used to efectively consume CPU resources. 
count4++;count2++;  // if not, then the time is definitely lost in scheduler, but then it means we DONT CARE otherwise we would use it for something :)
count1++;count4++;
count2++;count3++;              
count3++;count1++; 
count4++;count2++; 

}

defineTask(task1)
void task1::setup(){ }
void task1::loop() { justCount32();}

defineTask(task2)
void task2::setup(){}
void task2::loop() { justCount32(); }


long timer;

void setup(){
  SCbegin(57600);
  
  SCpln("please wait 10sec...");

  mySCoop.start(1000); // initialize all tasks with a total 1000us cycle time
  
  timer = millis();
}
long x,y,z,t;

void loop()
{ 
while(1) { // we do that so that we are not impacted by the potential yield() already included in teensy library

// select between the two lines below to run the 2 task OR to run only the counting for reference
  mySCoop.cycle(); // execute task 1 & task 2 during time quantum (500us) and come back here in loop
// justCount32();justCount32();justCount32();justCount32();justCount32();justCount32();justCount32();justCount32();

 if ( (millis()-timer) >= 10000 ) { timer += 10000;  // every 10 seconds
    SCpln("counter1 + counter2 + counter3 + counter4 = sum   = ");
    SCp(count1);SCp("  + ");SCp(count2);SCp("  + ");SCp(count3);SCp("  + ");SCp(count4);SCp(" = ");SCpln(count1+count2+count3+count4);
    count1=count2=count3=0;count4=0;

#if SCOOPTIMEREPORT > 0
    z=task1.cycleMicros; y= (z / 100);
    SCp("cycle time = ");SCp(z>>SCOOPTIMEREPORT);SCpln("us =100%");
    SCp("task1: ");SCp(task1.stackLeft());SCp(", time = ");SCp((x=(long)task1.yieldMicros)>>SCOOPTIMEREPORT); SCp("us =%");SCpln(x/y);  t=x;
    SCp("task2: ");SCp(task2.stackLeft());SCp(", time = ");SCp((x=(long)task2.yieldMicros)>>SCOOPTIMEREPORT); SCp("us =%");SCpln(x/y);  t+=x; t=z-t;
    SCp("SCoop time = ");SCp((t)>>SCOOPTIMEREPORT);SCp("us =%");SCpln(t/y);
#else
   SCpln(("you must setup SCOOPTIMEREPORT with a value between 1 and 4 to include timing variable and average time measurement"));
   while (1) ;
#endif    

 }
}
}


