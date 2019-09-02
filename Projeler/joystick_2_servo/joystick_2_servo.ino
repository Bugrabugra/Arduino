// Servo - Version: Latest

#include <Servo.h>

Servo servo1;
Servo servo2;

const int joyX = 0;
const int joyY = 1;
const int pinSwitch = 2;

int valueX = 0;
int valueY = 0;
int valueSwitch = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(pinSwitch, INPUT);

  servo1.attach(9);
  servo2.attach(11);

  servo1.write(0);
  servo2.write(0);
}

void loop()
{
  valueX = analogRead(joyX);
  valueX = map (valueX, 0, 1023, 0, 180);
  servo1.write(valueX);

  valueY = analogRead(joyY);
  valueY = map (valueY, 0, 1023, 0, 180);
  servo2.write(valueY);

  valueSwitch = digitalRead(pinSwitch);

  delay(15);
}
