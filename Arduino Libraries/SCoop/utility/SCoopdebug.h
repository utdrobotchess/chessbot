#ifndef SCOOPPDEBUG_H
#define SCOOPPDEBUG_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
//#error "why dont you use Arduino > 1.0  ?"
#endif

// a couple of macro to simplify the syntax of Serial.print ...

#define SCbegin(X)   Serial.begin(X);while(!Serial);delay(500)

#define SCp          Serial.print
#define SCphex(X)    Serial.print(X,HEX)
#define SCpms        Serial.print(millis());Serial.print(" ");Serial.print
#define SCpln        Serial.println
#define SCplnhex(X)  Serial.println(X,HEX)
#define SCpmsln      Serial.print(millis());Serial.print(" ");Serial.println

#define SCpkey1(X)   Serial.print("<");Serial.print(X); SCkey();

#define SCpkey2(X,Y)  Serial.print("<");Serial.print(X);Serial.print(":");Serial.print(Y,HEX);SCkey();

extern uint8_t SCkey();                   // wait for a key in the rx buff
extern void SChello();                    // print "ready...?" in a loop and wait for a key in the rx buffer. then print hello
extern void SCdump(uint8_t * ptr, int N); // print a list of byte in hex
extern void SCpulse(int time);            // pulse the builtin led once the led off for a certain time
extern void SCblinkonce(int time);        // almost same but square wave

#endif

