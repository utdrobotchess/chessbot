#ifndef SCOOPSWITCH_H
#define SCOOPSWITCH_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
//#error "why dont you use Arduino > 1.0  ?"
#endif


#if defined(__MK20DX128__) || defined (__SAM3X8E__)     // below code enables portability cross ARM / AVR

#define SCoopDefaultStackSize 256
typedef uint64_t SCoopStack_t __attribute__ ((aligned (8)));

#define SCoopSizeEnv 56         // 56 bytes = 13 register + return adress
extern void SCoopSwitch(uint8_t **newSP, uint8_t **oldSP);

extern uint32_t SCoopGetSP();  // used only in the trace function
#define SCoopSetSP(X) { register uint32_t val=X; asm volatile ("mov     sp, %0" : : "r" (val)); }   // not used in fact
#define SCoopSei()   asm volatile("CPSIE i")
#define SCoopCli()   asm volatile("CPSID i")
// these 3 definition are voluntary empty, so no code is generated if ARM compiler is used
#define SCoop_AVR_SREG(sreg)
#define SCoop_AVR_SREG_CLI(sreg)
#define SCoop_AVR_sreg_register

#else

#define SCoopDefaultStackSize 256   // could be reduced to 160 or even 128 without too much risks
typedef uint8_t SCoopStack_t;

#define SCoopSizeEnv 20             // 20 bytes = 18 registers + return adress
extern void SCoopSwitch(void *newSP, void *oldSP) __attribute__((naked));

#define SCoopGetSP() (uint16_t)SP   // direct access to SP register is possible
#define ScoopSetSP(X) SP = X
#define SCoopSei()    sei()      
#define SCoopCli()    cli()      
// these 3 definition simplifiy the atomic code creation in the library (SCoopFifo object only for the moment)
#define SCoop_AVR_sreg_register   register uint8_t sreg;
#define SCoop_AVR_SREG_CLI(sreg)  sreg = SREG; cli()  
#define SCoop_AVR_SREG(sreg)      SREG = sreg  
#endif

#endif



