#include <Wire.h>
#include <EEPROM.h>

byte botID = 29;

void setup()
{
    Serial.begin(57600);
    EEPROM.write(1,botID);
}

void loop()
{
    EEPROMReadWriteTest();
}

void EEPROMReadWriteTest()
{
    if(Serial.available() > 0)
    {
        char input = Serial.read();
        EEPROM.write(1, input);
    }

    Serial.println(EEPROM.read(1));
    delay(500);
}
