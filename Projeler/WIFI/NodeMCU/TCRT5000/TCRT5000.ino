int pinIR = 5;

void setup() 
{
  Serial.begin(9600);
  pinMode(pinIR, INPUT);
}

void loop() 
{
  Serial.println(digitalRead(pinIR));
  delay(100);
}