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
  Serial.print(gyro.ReturnXAngle());
  Serial.print(" ");
  Serial.print(gyro.ReturnYAngle());
  Serial.print(" ");
  Serial.print(gyro.ReturnZAngle());
  Serial.println(".");
}
