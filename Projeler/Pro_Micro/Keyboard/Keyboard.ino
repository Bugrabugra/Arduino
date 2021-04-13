int button = 3;

void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
}


void loop() {
  Serial.println(digitalRead(button));
  delay(1000);
}