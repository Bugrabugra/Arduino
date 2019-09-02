#include <Key.h>
#include "SevSeg.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

SevSeg sevseg1;
SevSeg sevseg2;
SevSeg sevseg3;

char* password = "1985B"; //create a password
int pozisyon = 0; //keypad position

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns

char keys[ROWS][COLS] =
{
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 18, 19, 20, 21 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 14, 15, 16, 17 }; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int pinRedLED1 = 22;
int pinRedLED2 = 24;
int pinRedLED3 = 26;
int pinGreenLED1 = 28;
int pinGreenLED2 = 30;
int pinGreenLED3 = 32;

int pinJackYellow = 37;
int pinRedLEDJackYellow = 9;
int pinGreenLEDJackYellow = 8;

int pinJackGreen = 10;
int pinRedLEDJackGreen = 7;
int pinGreenLEDJackGreen = 6;

int potValue1 = 0;
int potValue2 = 0;
int potValue3 = 0;

int pinRedAlarmButton = 11;
int pinOldButton = 12;

int pinKeypadStartButton = 34;

bool cablesState = false;
bool valueState = false;
bool redbuttonState = false;
bool keypadButtonState = false;

char key;
String keyEnterString = "     ";
int keyEnterNo = 0;

void setup()
{
  setLocked(true);

  lcd.init();
  lcd.backlight();

  byte numDigits = 1;
  byte digitPins[] = { 14 };
  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_ANODE;

  byte segmentPins1[] = { 49, 47, 41, 43, 45, 53, 51, 39 };
  sevseg1.begin(hardwareConfig, numDigits, digitPins, segmentPins1, resistorsOnSegments);
  sevseg1.setBrightness(50);

  byte segmentPins2[] = { 48, 46, 40, 42, 44, 52, 50, 38 };
  sevseg2.begin(hardwareConfig, numDigits, digitPins, segmentPins2, resistorsOnSegments);
  sevseg2.setBrightness(50);

  byte segmentPins3[] = { 4, 5, 33, 35, 36, 2, 3, 31 };
  sevseg3.begin(hardwareConfig, numDigits, digitPins, segmentPins3, resistorsOnSegments);
  sevseg3.setBrightness(50);

  pinMode(pinRedLED1, OUTPUT);
  pinMode(pinRedLED2, OUTPUT);
  pinMode(pinRedLED3, OUTPUT);
  pinMode(pinGreenLED1, OUTPUT);
  pinMode(pinGreenLED2, OUTPUT);
  pinMode(pinGreenLED3, OUTPUT);

  pinMode(pinJackYellow, INPUT_PULLUP);
  pinMode(pinRedLEDJackYellow, OUTPUT);
  pinMode(pinGreenLEDJackYellow, OUTPUT);

  pinMode(pinJackGreen, INPUT_PULLUP);
  pinMode(pinRedLEDJackGreen, OUTPUT);
  pinMode(pinGreenLEDJackGreen, OUTPUT);

  pinMode(pinRedAlarmButton, INPUT_PULLUP);
  pinMode(pinOldButton, INPUT_PULLUP);

  pinMode(pinKeypadStartButton, OUTPUT);
}


void loop()
{
  cablesState = false;
  valueState = false;
  redbuttonState = false;
  keypadButtonState = false;

  if (digitalRead(pinRedAlarmButton) == 0)
  {
    digitalWrite(pinRedLED1, HIGH);
    digitalWrite(pinRedLED2, HIGH);
    digitalWrite(pinRedLED3, HIGH);
    delay(500);
    digitalWrite(pinRedLED1, LOW);
    digitalWrite(pinRedLED2, LOW);
    digitalWrite(pinRedLED3, LOW);
    delay(500);
  }
  else
  {
    digitalWrite(pinRedLED3, HIGH);
    digitalWrite(pinGreenLED3, LOW);
    redbuttonState = true;
  }


  //sarı erkek
  if (digitalRead(pinJackYellow) == 1)
  {
    digitalWrite(pinRedLEDJackYellow, HIGH);
    digitalWrite(pinGreenLEDJackYellow, LOW);
  }
  else
  {
    digitalWrite(pinRedLEDJackYellow, LOW);
    digitalWrite(pinGreenLEDJackYellow, HIGH);
  }

  //yeşil erkek

  if (digitalRead(pinJackGreen) == 0 && digitalRead(pinJackYellow) == 0 && digitalRead(pinOldButton) == 0)
  {
    digitalWrite(pinRedLEDJackGreen, LOW);
    digitalWrite(pinGreenLEDJackGreen, HIGH);
    digitalWrite(pinGreenLED1, HIGH);
    digitalWrite(pinRedLED1, LOW);
    cablesState = true;
  }
  else
  {
    digitalWrite(pinRedLEDJackGreen, HIGH);
    digitalWrite(pinGreenLEDJackGreen, LOW);
    digitalWrite(pinGreenLED1, LOW);
    digitalWrite(pinRedLED1, HIGH);
    digitalWrite(pinRedLED2, HIGH);
    digitalWrite(pinGreenLED2, LOW);
  }


  potValue1 = analogRead(A0);
  int mappedPotValue1 = map(potValue1, 0, 1024, 0, 10);

  sevseg1.setNumber(mappedPotValue1);
  sevseg1.refreshDisplay();

  potValue2 = analogRead(A1);
  int mappedPotValue2 = map(potValue2, 0, 1024, 0, 10);

  potValue3 = analogRead(A2);
  int mappedPotValue3 = map(potValue3, 0, 1024, 0, 10);

  // keypad start button
  if (digitalRead(pinKeypadStartButton) == 1)
  {
    keypadButtonState = true;
  }


  if (mappedPotValue1 < 6 && cablesState && redbuttonState)
  {
    sevseg2.setNumber(mappedPotValue2);
    sevseg2.refreshDisplay();

    if (mappedPotValue2 == 8)
    {
      sevseg3.setNumber(mappedPotValue3);
      sevseg3.refreshDisplay();

      if (mappedPotValue3 < 2)
      {
        digitalWrite(pinGreenLED2, HIGH);
        digitalWrite(pinRedLED2, LOW);

        valueState = true;

        if (keypadButtonState)
        {
          char keypadChar = keypad.getKey();
          if (keypadChar)
          {
            key = keypadChar;
            keyEnterString[keyEnterNo] = keypadChar;

            if (key == 'C')
            {
              keyEnterNo = 0;
              keyEnterString = "     ";
              pozisyon = 0;
              setLocked(true);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("RESETLENDI!");
              delay(1000);
            }
            else if (keyEnterNo == 5)
            {
              for (int i = 0; i < 5; i++)
              {
                if (keyEnterString[i] == password[i])
                {
                  pozisyon++;
                }
              }

              if (pozisyon == 5)
              {
                keyEnterString = "     ";
                setLocked(false);
                lcd.clear();
                digitalWrite(pinRedLED3, LOW);
                digitalWrite(pinGreenLED3, HIGH);
                lcd.setCursor(0, 0);
                lcd.print(" SIFRE DOGRU :) ");
                delay(10000);
                keyEnterNo = 0;

                pozisyon = 0;
              }
              else
              {
                keyEnterString = "     ";
                pozisyon = 0;
                keyEnterNo = 0;
                setLocked(false);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(" YANLIS SIFRE! ");
                lcd.setCursor(0, 1);
                lcd.print("TEKRAR DENEYINIZ");
                digitalWrite(pinRedLED3, HIGH);
                digitalWrite(pinGreenLED3, LOW);
                delay(5000);
              }
            }
            else
            {
              keyEnterNo++;
            }
          }
        }
      }
      else
      {
        digitalWrite(pinGreenLED2, LOW);
        digitalWrite(pinRedLED2, HIGH);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("KABLOLAR TAMAM!");
        lcd.setCursor(0, 1);
        lcd.print("KONTROL:DEGERLER");
      }
    }
    else
    {
      sevseg3.setNumber(0);
      sevseg3.refreshDisplay();
      delay(100);
      digitalWrite(pinGreenLED2, LOW);
      digitalWrite(pinRedLED2, HIGH);
    }
  }

  if (cablesState && redbuttonState && valueState && keypadButtonState)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SIFRE: ");

    if (key)
    {
      lcd.setCursor(0, 1);
      lcd.print(keyEnterString);
    }
  }
  else if (cablesState && redbuttonState && valueState && !keypadButtonState)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DEGERLER TAMAM!");
    lcd.setCursor(0, 1);
    lcd.print("KONTROL: TUSLAR!");
  }
  else if (cablesState && redbuttonState)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KABLOLAR TAMAM!");
    lcd.setCursor(0, 1);
    lcd.print("KONTROL:DEGERLER");
  }
  else if (!redbuttonState)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SISTEM HATASI!");
    lcd.setCursor(0, 1);
    lcd.print("SALTERE BASINIZ!");
  }
  else if (!cablesState)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SISTEM HATASI!");
    lcd.setCursor(0, 1);
    lcd.print("KONTROL: KABLO!");
  }
  else
  {
    sevseg2.setNumber(0);
    sevseg2.refreshDisplay();
    delay(100);
    digitalWrite(pinGreenLED2, LOW);
    digitalWrite(pinRedLED2, HIGH);
  }
}

void setLocked(int locked)
{
  if (locked)
  {
    digitalWrite(pinGreenLED3, LOW);
  }
  else
  {
    digitalWrite(pinGreenLED3, HIGH);
  }
}
