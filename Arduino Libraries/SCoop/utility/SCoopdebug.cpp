
#include "SCoopdebug.h"


void SCdump(uint8_t * ptr, int N)
{
  for (int i=0; i <N; i++) {
    uint8_t c = *(ptr+i);
    if (c<16) { SCp("0"); };
    Serial.print(c,HEX);
    SCp(" ");
  }
  SCpln("");
};


void SCblinkonce(int time)
{
digitalWrite(LED_BUILTIN,HIGH); delay(time); digitalWrite(LED_BUILTIN,LOW); delay(time);
};

void SCpulse(int time)
{while(1) {
  digitalWrite(LED_BUILTIN,HIGH); delay(50); digitalWrite(LED_BUILTIN,LOW); delay(time); }
};


void SChello()
{
 SCbegin(115200); while (!Serial);
 do { SCpln("ready to start ? press any key"); SCblinkonce(50); } 
 while (!Serial.available()) ;
 char cc;
 cc= Serial.read();
 SCpln("hello"); delay(100);

};

uint8_t SCkey()
{
  Serial.print("> ?");
  while (!Serial.available()) {
    SCblinkonce(100); };
  char cc;
  cc =Serial.read();  
  SCpln("");
  delay(100);

};

