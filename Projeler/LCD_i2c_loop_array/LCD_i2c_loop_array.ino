#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int button = 7;
char* kelimeler[] = {"CANIM", "ANNEM", "SENI", "COK", "SEVIYORUM", "=)"};


void setup()
{
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(button, INPUT_PULLUP);
}


void loop()
{
  int  index = 0;
  while (index < 6)
  {
    if (digitalRead(button) == LOW)
    {
      delay(300);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(kelimeler[index]);
      index = index + 1;
    }
  }
}
