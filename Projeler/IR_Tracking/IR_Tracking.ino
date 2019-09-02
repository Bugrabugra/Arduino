#define sens1 9
#define sens2 10
#define sens3 11
#define sens4 12

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

  int sol1 = digitalRead(sens1);
  int sol2 = digitalRead(sens2);
  int sol3 = digitalRead(sens3);
  int sol4 = digitalRead(sens4);

  if (sol1 == LOW)
  {
    Serial.println("Sol 1'de obje var");
  }

  if (sol2 == LOW)
  {
    Serial.println("Sol 2'de obje var");
  }

  if (sol3 == LOW)
  {
    Serial.println("Sol 3'te obje var");
  }

  if (sol4 == LOW)
  {
    Serial.println("Sol 4'te obje var");
  }
}
