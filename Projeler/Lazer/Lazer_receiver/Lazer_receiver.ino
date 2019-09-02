int pinDetector = 7;

void setup()
{
  Serial.begin(9600);
  pinMode(pinDetector, INPUT);
}

void loop()
{
  boolean val = digitalRead(pinDetector);
  Serial.println(val);
}
