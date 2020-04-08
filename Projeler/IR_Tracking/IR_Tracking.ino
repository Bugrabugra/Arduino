int sens1 = 5;
int sens2 = 6;
int sens3 = 7;
int sens4 = 8;

void setup()
{
  Serial.begin(9600);

  pinMode(sens1, INPUT);
  pinMode(sens2, INPUT);
  pinMode(sens3, INPUT);
  pinMode(sens4, INPUT);
}

void loop()
{

  int valSensor1 = digitalRead(sens1);
  int valSensor2 = digitalRead(sens2);
  int valSensor3 = digitalRead(sens3);
  int valSensor4 = digitalRead(sens4);

  if (valSensor1 == LOW)
  {
    Serial.println("Sol 1'de obje var");
    delay(1000);
  }

  if (valSensor2 == LOW)
  {
    Serial.println("Sol 2'de obje var");
    delay(1000);
  }

  if (valSensor3 == LOW)
  {
    Serial.println("Sol 3'te obje var");
    delay(1000);
  }

  if (valSensor4 == LOW)
  {
    Serial.println("Sol 4'te obje var");
    delay(1000);
  }
}
