const int pinAnahtar = 2;
const int pinLed = 7;
int anahtarDurumu = 0;

void setup()
{
  pinMode(pinAnahtar, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  anahtarDurumu = digitalRead(pinAnahtar);

  if (anahtarDurumu == LOW)
  {
    digitalWrite(pinLed, HIGH);
  }
  else
  {
    digitalWrite(pinLed, LOW);
  }
}
