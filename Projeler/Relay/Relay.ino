int pinButton = 3;
int pinRelay1 = 6;
int pinRelay2 = 7;
bool stateButton;

void setup() {
  Serial.begin(9600);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);
  stateButton = false;
}


void loop() {
  if (digitalRead(pinButton) == LOW) {
    if (stateButton == false) {
      stateButton = true;
      digitalWrite(pinRelay1, HIGH);
      digitalWrite(pinRelay2, HIGH);
      Serial.println("On");
      delay(500);
    } else {
      stateButton = false;
      digitalWrite(pinRelay1, LOW);
      digitalWrite(pinRelay2, LOW);
      Serial.println("Off");
      delay(500);
    }
  }
}
