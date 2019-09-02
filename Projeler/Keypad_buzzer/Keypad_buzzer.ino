#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] =
{
  { '1', '2', '3', '4' },
  { '5', '6', '7', '8' },
  { '9', '0', 'A', 'B' },
  { 'C', 'D', 'E', 'F' }
};
byte rowPins[ROWS] = { 5, 4, 3, 2 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 9, 8, 7, 6 }; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  char key = keypad.getKey();

  if (key)
  {
    Serial.println(key);

    switch (key) {
      case '1':
        tone(12, 200);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '2':
        tone(12, 300);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '3':
        tone(12, 400);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '4':
        tone(12, 500);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '5':
        tone(12, 600);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '6':
        tone(12, 700);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '7':
        tone(12, 800);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '8':
        tone(12, 900);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '9':
        tone(12, 1000);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case '0':
        tone(12, 1100);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case 'A':
        tone(12, 1200);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case 'B':
        tone(12, 1300);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case 'C':
        tone(12, 1400);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case 'D':
        tone(12, 1500);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case 'E':
        tone(12, 1600);
        delay(100);
        noTone(12);
        delay(100);
        break;
      case 'F':
        tone(12, 1700);
        delay(100);
        noTone(12);
        delay(100);
        break;
    }
  }
}
