const int pinHareket = 10;
const int pinSes = 2;

void setup() {
  Serial.begin(9600);
  pinMode(pinHareket, INPUT);
  pinMode(pinSes, OUTPUT);
}

void loop() {
  if ((digitalRead(pinHareket)) == HIGH) {
    Serial.println("Hareket");
    tone(pinSes, 300);
    delay(1000);
    noTone(pinSes);
  } else {
    Serial.println("Bir sey yok");
    delay(1000);
  }
}
