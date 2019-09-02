const int joyX = 0;
const int joyY = 1;
const int pinSwitch = 2;
int valueX = 0;
int valueY = 0;
int valueSwitch = 0;

void setup()
{
  pinMode(pinSwitch, INPUT);
  digitalWrite(pinSwitch, HIGH);
  Serial.begin(115200);
}

void loop()
{
  valueX = analogRead(joyX);
  delay(100);
  valueY = analogRead(joyY);
  delay(100);
  valueSwitch = digitalRead(pinSwitch);

  Serial.print("Switch: ");
  Serial.print(valueSwitch);
  Serial.print("\n");
  Serial.print("X-switch: ");
  Serial.print(valueX);
  Serial.print("\n");
  Serial.print("Y-switch: ");
  Serial.print(valueY);
  Serial.print("\n\n");
  delay(2000);
}
