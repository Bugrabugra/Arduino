//Arduino ile Python haberleşmesi için yazılan kod//

const int pinRedButton = 7;
const int pinGreenButton = 4;
const int pinBlueButton = 2;
const int pinConfirmButton = 8;
const int pinPotans = A0;

int buttonStateRed = 0;
int buttonStateGreen = 0;
int buttonStateBlue = 0;
int buttonStateConfirm = 0;
int potansValue = 0;

void setup() {
  Serial.begin(9600);

  pinMode(pinRedButton, INPUT);
  pinMode(pinGreenButton, INPUT);
  pinMode(pinBlueButton, INPUT);
  pinMode(pinConfirmButton, INPUT);
}

void loop() {

  buttonStateRed = digitalRead(pinRedButton);
  buttonStateGreen = digitalRead(pinGreenButton);
  buttonStateBlue = digitalRead(pinBlueButton);
  buttonStateConfirm = digitalRead(pinConfirmButton);
  potansValue = map(analogRead(pinPotans), 0, 1023, 0, 255);

  Serial.println(potansValue);
  delay(300);

  if (buttonStateRed == HIGH) {
    delay(300);
    Serial.println("Red");
  }

  if (buttonStateGreen == HIGH) {
    delay(300);
    Serial.println("Green");
  }

  if (buttonStateBlue == HIGH) {
    delay(300);
    Serial.println("Blue");
  }

  if (buttonStateConfirm == HIGH) {
    delay(300);
    Serial.println("Confirm");
  }
}
