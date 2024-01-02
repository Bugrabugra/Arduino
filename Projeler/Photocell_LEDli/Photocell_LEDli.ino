const int pinPC = A0;
const int pinLED1 = 8;
const int pinLED2 = 9;
const int pinLED3 = 10;
const int pinLED4 = 11;
int okunan_deger = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinPC, INPUT);
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(pinLED3, OUTPUT);
  pinMode(pinLED4, OUTPUT);
}

void loop() {
  okunan_deger = analogRead(pinPC);

  if (okunan_deger > 300) {
    digitalWrite(pinLED1, HIGH);
  }
  if (okunan_deger > 600) {
    digitalWrite(pinLED2, HIGH);
  }
  if (okunan_deger > 900) {
    digitalWrite(pinLED3, HIGH);
  }
  if (okunan_deger > 1200) {
    digitalWrite(pinLED4, HIGH);
  }
  if (okunan_deger < 300) {
    digitalWrite(pinLED1, LOW);
  }
  if (okunan_deger < 600) {
    digitalWrite(pinLED2, LOW);
  }
  if (okunan_deger < 900) {
    digitalWrite(pinLED3, LOW);
  }
  if (okunan_deger < 1200) {
    digitalWrite(pinLED4, LOW);
  }

  Serial.println(okunan_deger);
  delay(1000);
}
