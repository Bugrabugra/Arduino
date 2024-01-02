//LED pinleri
int pinRedLED = 2;
int pinGreenLED = 3;
int pinBlueLED = 4;

//Düğme pinleri
int btnRed = 5;
int btnGreen = 6;
int btnBlue = 7;

//Düğme değerleri
int valBtnRed = 0;
int valBtnGreen = 0;
int valBtnBlue = 0;

//Düğme son durumları
int valBtnRedLastState = 0;
int valBtnGreenLastState = 0;
int valBtnBlueLastState = 0;

//Potansiyometre pini
int pinPot = 0;

//Potansiyometre düğme değeri
int potDeger = 0;

void setup() {
  Serial.begin(9600);

  pinMode(pinRedLED, OUTPUT);
  pinMode(pinGreenLED, OUTPUT);
  pinMode(pinBlueLED, OUTPUT);

  pinMode(btnRed, INPUT_PULLUP);
  pinMode(btnGreen, INPUT_PULLUP);
  pinMode(btnBlue, INPUT_PULLUP);
}

void loop() {
  valBtnRed = digitalRead(btnRed);
  valBtnGreen = digitalRead(btnGreen);
  valBtnBlue = digitalRead(btnBlue);

  Serial.println("valBtnRed: ");
  Serial.println(valBtnRed);

  Serial.println("stateRedLED: ");
  Serial.println(!valBtnRedLastState);

  if (valBtnRed == LOW) {
    if (valBtnRedLastState == 0) {
      delay(200);
      digitalWrite(pinRedLED, HIGH);
      valBtnRedLastState = 1;
    } else if (valBtnRedLastState == 1) {
      delay(200);
      digitalWrite(pinRedLED, LOW);
      valBtnRedLastState = 0;
    }
  }
}
