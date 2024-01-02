int lm35Pin = A0;

void setup() {
  analogReference(INTERNAL);
  Serial.begin(9600);
}

void loop() {
  int sicaklikVolt = analogRead(lm35Pin);
  float sicaklikC = sicaklikC = sicaklikVolt / 9.31;

  Serial.println(sicaklikC);
  delay(1000);
}
