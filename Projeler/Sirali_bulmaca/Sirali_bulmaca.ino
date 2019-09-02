//LED pinleri
int pinRedLED = 4;
int pinGreenLED = 3;
int pinBlueLED = 2;

//RGB LED pinleri
int pinRGBRedLED = 9;
int pinRGBGreenLED = 10;
int pinRGBBlueLED = 11;

//Düğme pinleri
int btnRed = 5;
int btnGreen = 6;
int btnBlue = 7;

//Buzzer pini
int pinBuzzer = 12;

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

void setup()
{
  Serial.begin(9600);

  pinMode(pinRedLED, OUTPUT);
  pinMode(pinGreenLED, OUTPUT);
  pinMode(pinBlueLED, OUTPUT);
  pinMode(pinRGBRedLED, OUTPUT);
  pinMode(pinRGBGreenLED, OUTPUT);
  pinMode(pinRGBBlueLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  pinMode(btnRed, INPUT_PULLUP);
  pinMode(btnGreen, INPUT_PULLUP);
  pinMode(btnBlue, INPUT_PULLUP);
}

void loop()
{
  valBtnRed = digitalRead(btnRed);
  valBtnGreen = digitalRead(btnGreen);
  valBtnBlue = digitalRead(btnBlue);

  if (valBtnRed == LOW)
  {
    if (valBtnRedLastState == 0)
    {
      delay(200);
      digitalWrite(pinRedLED, HIGH);
      digitalWrite(pinRGBRedLED, HIGH);
      valBtnRedLastState = 1;
    }
    else if (valBtnRedLastState == 1)
    {
      delay(200);
      digitalWrite(pinRedLED, LOW);
      digitalWrite(pinRGBRedLED, LOW);
      digitalWrite(pinRGBGreenLED, LOW);
      digitalWrite(pinRGBBlueLED, LOW);
      valBtnRedLastState = 0;
    }
  }

  if (valBtnRedLastState == 1)
  {
    if (valBtnGreen == LOW)
    {
      if (valBtnGreenLastState == 0)
      {
        delay(200);
        digitalWrite(pinGreenLED, HIGH);
        digitalWrite(pinRGBGreenLED, HIGH);
        valBtnGreenLastState = 1;
      }
      else if (valBtnGreenLastState == 1)
      {
        delay(200);
        digitalWrite(pinGreenLED, LOW);
        digitalWrite(pinRGBRedLED, LOW);
        digitalWrite(pinRGBGreenLED, LOW);
        digitalWrite(pinRGBBlueLED, LOW);
        valBtnGreenLastState = 0;
      }
    }
  }

  if (valBtnGreenLastState == 1)
  {
    if (valBtnBlue == LOW)
    {
      if (valBtnBlueLastState == 0)
      {
        delay(200);
        digitalWrite(pinBlueLED, HIGH);
        digitalWrite(pinRGBBlueLED, HIGH);
        valBtnBlueLastState = 1;
      }
      else if (valBtnBlueLastState == 1)
      {
        delay(200);
        digitalWrite(pinBlueLED, LOW);
        digitalWrite(pinRGBRedLED, LOW);
        digitalWrite(pinRGBGreenLED, LOW);
        digitalWrite(pinRGBBlueLED, LOW);
        valBtnBlueLastState = 0;
      }
    }
  }

  potDeger = analogRead(pinPot);
  Serial.println(potDeger);

  if (valBtnBlueLastState == 1)
  {
    if (potDeger > 600 && potDeger < 800)
    {
      digitalWrite(pinRGBGreenLED, HIGH);
      tone(pinBuzzer, 1000);
      delay(100);
      noTone(pinBuzzer);
      delay(100);
    }
    else
    {
      digitalWrite(pinRGBGreenLED, LOW);
      digitalWrite(pinRGBRedLED, LOW);
      digitalWrite(pinRGBGreenLED, LOW);
      digitalWrite(pinRGBBlueLED, LOW);
      noTone(pinBuzzer);
    }
  }
}
