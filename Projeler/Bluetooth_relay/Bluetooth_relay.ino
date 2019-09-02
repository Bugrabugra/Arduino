int pinRelay = 2;
String veri = "";

void setup()
{
  Serial.begin(9600);
  pinMode(pinRelay, OUTPUT);
}

void loop()
{
  veri = "";

  while (Serial.available())
  {
    veri += (char)Serial.read();

    if (veri == "1")
    {
      digitalWrite(pinRelay, HIGH);
    }
    else if (veri == "2")
    {
      digitalWrite(pinRelay, LOW);
    }
  }
}
