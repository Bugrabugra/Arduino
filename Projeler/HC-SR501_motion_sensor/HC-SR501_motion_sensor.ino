int ledDetected = 13;
int ledReady = 12;
int ledWait = 11;

int pinPir = 7;

int motionDetected = 0;

int pirValue;

void setup()
{
  pinMode(ledDetected, OUTPUT);
  pinMode(ledReady, OUTPUT);
  pinMode(ledWait, OUTPUT);

  pinMode(pinPir, INPUT);

  digitalWrite(ledDetected, LOW);
  digitalWrite(ledReady, LOW);
  digitalWrite(ledWait, HIGH);

  delay(60000);

  digitalWrite(ledReady, HIGH);
  digitalWrite(ledWait, LOW);
}

void loop()
{
  pirValue = digitalRead(pinPir);

  if (pirValue == 1)
  {
    digitalWrite(ledDetected, HIGH);
    motionDetected = 1;

    delay(3000);
  }
  else
  {
    digitalWrite(ledDetected, LOW);
  }

  if (motionDetected == 1)
  {
    digitalWrite(ledDetected, LOW);
    digitalWrite(ledReady, LOW);
    digitalWrite(ledWait, HIGH);

    delay(6000);

    digitalWrite(ledReady, HIGH);
    digitalWrite(ledWait, LOW);

    motionDetected = 0;
  }
}
