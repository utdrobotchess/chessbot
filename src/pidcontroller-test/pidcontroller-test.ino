#include <Wire.h>
#include <Gyroscope.h>
#include <PIDController.h>

PIDController  PController(10,1,0,0,255, -255);
PIDController  IController(10,0,1,0,255,-255);
PIDController  DController(10,0,0,1,255,-255);
PIDController  PIDController(10,1,1,1,255,-255);

Gyroscope gyro;

void setup()
{
  Serial.begin(9600);
  gyro.Init();
}

void loop()
{

}

void PIDControllerOutputTest()
{
  delay(10);
  gyro.UpdateAngles();
  Serial.print(PController.ComputeOutput(gyro.anglesXYZ[2], 0));
  Serial.print(" ");
  Serial.print(IController.ComputeOutput(gyro.anglesXYZ[2], 0));
  Serial.print(" ");
  Serial.print(DController.ComputeOutput(gyro.anglesXYZ[2], 0));
  Serial.print(" ");
  Serial.print(PIDController.ComputeOutput(gyro.anglesXYZ[2], 0));
  Serial.println();
}
