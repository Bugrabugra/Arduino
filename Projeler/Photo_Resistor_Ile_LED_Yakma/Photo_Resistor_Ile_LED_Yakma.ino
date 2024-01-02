int pinPR = 0;
int pinLED1 = 2;
int pinLED2 = 3;
int pinLED3 = 4;
int pinLED4 = 5;
int analogOkuma = 0;


void setup() {
  pinMode(pinPR, INPUT);
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(pinLED3, OUTPUT);
  pinMode(pinLED4, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  analogOkuma = analogRead(pinPR);

  Serial.println(analogOkuma);
  delay(1000);
}
