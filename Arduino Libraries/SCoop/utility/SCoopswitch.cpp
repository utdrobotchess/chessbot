
#include "SCoopswitch.h"

/********* ASSEMBLY ******************************************************/
// original ideas taken out from ChibiOS, slightly modified. Credit to the author
// http://forum.pjrc.com/threads/540-ChibiOS-RTand-FreeRTOS-for-Teensy-3-0
//************************************************************************/

#if defined(__MK20DX128__) || defined (__SAM3X8E__)// ARM platform / teensy 3 / Arduino DUE (tbd)

__attribute__((always_inline)) uint32_t SCoopGetSP() { register uint32_t val; asm volatile ("mov     %0,sp" : "=r" (val)); return val; } ;

void SCoopSwitch(uint8_t **newSP, uint8_t **oldSP) 
{ asm volatile ("push    {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}" : : : "memory");
  asm volatile ("str     sp, [%1, #0]  \n\t"
                "ldr     sp, [%0, #0]" : : "r" (newSP), "r" (oldSP));
  asm volatile ("pop     {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, pc}" : : : "memory");
};

#else // AVR platform / teensy2 / Arduino UNO ...

void SCoopSwitch(void *newSP, void *oldSP)
{ asm volatile ("push    r2");   asm volatile ("push    r3");   asm volatile ("push    r4");
  asm volatile ("push    r5");   asm volatile ("push    r6");   asm volatile ("push    r7");
  asm volatile ("push    r8");   asm volatile ("push    r9");   asm volatile ("push    r10");
  asm volatile ("push    r11");  asm volatile ("push    r12");  asm volatile ("push    r13");
  asm volatile ("push    r14");  asm volatile ("push    r15");  asm volatile ("push    r16");
  asm volatile ("push    r17");  asm volatile ("push    r28");  asm volatile ("push    r29");

  asm volatile ("movw    r28, r22");
  asm volatile ("in      r2, 0x3d");
  asm volatile ("std     Y+0, r2");  // store the current SP into the pointer oldSP
  asm volatile ("in      r2, 0x3e");
  asm volatile ("std     Y+1, r2");

  asm volatile ("movw    r28, r24");
  asm volatile ("ldd     r2, Y+0");  // restore the SP from the pointer newSP
  asm volatile ("in      r3, 0x3f"); // save SREG
  asm volatile ("cli             ");
  asm volatile ("out     0x3d, r2");
  asm volatile ("ldd     r2, Y+1");
  asm volatile ("out     0x3e, r2");
  asm volatile ("out     0x3f, r3"); // restore SREG

  asm volatile ("pop     r29");   asm volatile ("pop     r28");   asm volatile ("pop     r17");
  asm volatile ("pop     r16");   asm volatile ("pop     r15");   asm volatile ("pop     r14");
  asm volatile ("pop     r13");   asm volatile ("pop     r12");   asm volatile ("pop     r11");
  asm volatile ("pop     r10");   asm volatile ("pop     r9");    asm volatile ("pop     r8");
  asm volatile ("pop     r7");    asm volatile ("pop     r6");    asm volatile ("pop     r5");
  asm volatile ("pop     r4");    asm volatile ("pop     r3");    asm volatile ("pop     r2");
  asm volatile ("ret");
};

/*  NOT USED
// number of uS when timer 0 is incremented by 1
#if F_CPU == 16000000L
  #define TIMER0_MICROS_INC  	4     // standard prescaler F /64
#elif F_CPU == 8000000L
  #define TIMER0_MICROS_INC  	8     // standard prescaler F /64
#elif F_CPU == 4000000L
  #define TIMER0_MICROS_INC  	16    // standard prescaler F /64
#elif F_CPU == 2000000L
  #define TIMER0_MICROS_INC  	32    // prescaler = 
#elif F_CPU == 1000000L
  #define TIMER0_MICROS_INC  	64    // prescaler =
#endif
*/

#endif

