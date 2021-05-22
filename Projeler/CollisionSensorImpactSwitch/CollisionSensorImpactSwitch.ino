int pinSensor = 9;

void setup() {
  pinMode(pinSensor, INPUT);
  Serial.begin(9600);
}

void loop() {
  delay(1000);
  Serial.println(digitalRead(pinSensor));
}