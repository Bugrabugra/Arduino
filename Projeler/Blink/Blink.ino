int pinButton = 2;

void setup() {
  Serial.begin(9600);
  pinMode(pinButton, INPUT_PULLUP);
}

void loop() {
  Serial.println(digitalRead(pinButton));
  delay(100);
}