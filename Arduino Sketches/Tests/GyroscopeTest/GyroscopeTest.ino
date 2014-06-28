#include <Wire.h>
#include <Gyroscope.h>

Gyroscope gyro;
float Angles[3];

void setup()
{
  Serial.begin(9600);
  gyro.Init();
}

void loop()
{
  angleEstimationTest();
}

void angleEstimationTest()
{
  delay(10);
  gyro.UpdateAngles();
  Serial.print(gyro.anglesXYZ[0]);
  Serial.print(" ");
  Serial.print(gyro.anglesXYZ[1]);
  Serial.print(" ");
  Serial.print(gyro.anglesXYZ[2]);
  Serial.println(".");
}
