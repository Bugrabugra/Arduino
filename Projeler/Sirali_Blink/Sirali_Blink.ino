int timer = 200;

void setup()
{
  for (int pinNo = 2; pinNo < 7 ; pinNo++)
  {
    pinMode(pinNo, OUTPUT);
  }
}

void loop()
{
  for (int pinNo = 2; pinNo < 7 ; pinNo++)
  {
    digitalWrite(pinNo, HIGH);
    delay(timer); // Wait for 1000 millisecond(s)
    digitalWrite(pinNo, LOW);
    delay(timer); // Wait for 1000 millisecond(s)
  }
  for (int pinNo = 6; pinNo > 1 ; pinNo--)
  {
    digitalWrite(pinNo, HIGH);
    delay(timer); // Wait for 1000 millisecond(s)
    digitalWrite(pinNo, LOW);
    delay(timer); // Wait for 1000 millisecond(s)
  }
}
