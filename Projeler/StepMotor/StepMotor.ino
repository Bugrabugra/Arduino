// CheapStepper - Version: Latest
#include <CheapStepper.h>

CheapStepper stepper (8, 9, 10, 11);

void setup()
{
  stepper.setRpm(12);
}

void loop()
{
  stepper.moveDegreesCW (180);
  delay(1000);
  stepper.moveDegreesCCW (180);
  delay(1000);
}
