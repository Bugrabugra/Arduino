#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int pinButtonDown = 3;
int pinButtonUp = 2;
int pinButtonEnter = 5;
int pinButtonBack = 4;

int pinGreenLED = 6;
int pinBlueLED = 7;

int pinBuzzer = 8;

int valButtonDown = 0;
int valButtonUp = 0;
int valButtonEnter = 0;
int valButtonBack = 0;

int cursorPos = 0;
int numPage = 0;


void setup()
{
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(pinButtonDown, INPUT_PULLUP);
  pinMode(pinButtonUp, INPUT_PULLUP);
  pinMode(pinButtonEnter, INPUT_PULLUP);
  pinMode(pinButtonBack, INPUT_PULLUP);
  pinMode(pinGreenLED, OUTPUT);
  pinMode(pinBlueLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
}

void loop()
{
  valButtonDown = digitalRead(pinButtonDown);
  valButtonUp = digitalRead(pinButtonUp);
  valButtonEnter = digitalRead(pinButtonEnter);
  valButtonBack = digitalRead(pinButtonBack);

  if (numPage == 0) // Main menu
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.cp437(true);
    display.setCursor(10, 0);
    display.print("Araclar");
    display.setCursor(10, 15);
    display.print("Uygulamalar");
    display.setCursor(10, 30);
    display.print("Rehber");
    display.setCursor(10, 45);
    display.print("LEDler");
  }

  if (numPage == 1) // Green LED and Blue LED screen 
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.cp437(true);
    display.setCursor(10, 0);
    display.print("Green LED");
    display.setCursor(10, 15);
    display.print("Blue LED");
  }

  if (numPage == 2) // Green LED screen
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.cp437(true);
    display.setCursor(10, 0);
    display.print("Green High");
    display.setCursor(10, 15);
    display.print("Green Low");
  }

  if (numPage == 3) // Blue LED screen
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.cp437(true);
    display.setCursor(10, 0);
    display.print("Blue High");
    display.setCursor(10, 15);
    display.print("Blue Low");
  }

  if (valButtonDown == 0 && numPage == 0)
  {
    delay(200);
    cursorPos = cursorPos + 15;
    buzzer();
    if (cursorPos > 45)
    {
      cursorPos = 0;
    }
  }

  if (valButtonUp == 0 && numPage == 0)
  {
    delay(200);
    cursorPos = cursorPos - 15;
    buzzer();
    if (cursorPos < 0)
    {
      cursorPos = 45;
    }
  }

  if (valButtonEnter == 0 && numPage == 0 && cursorPos == 45)
  {
    cursorPos = 0;
    numPage = 1;
    delay(200);
    buzzer();
  }

  if (valButtonDown == 0 && numPage == 1)
  {
    delay(200);
    cursorPos = cursorPos + 15;
    buzzer();
    if (cursorPos > 15)
    {
      cursorPos = 0;
    }
  }

  if (valButtonUp == 0 && numPage == 1)
  {
    delay(200);
    cursorPos = cursorPos - 15;
    buzzer();
    if (cursorPos < 0)
    {
      cursorPos = 15;
    }
  }

  if (valButtonEnter == 0 && numPage == 1 && cursorPos == 0) // Green High and Low
  {
    delay(500);
    cursorPos = 0;
    numPage = 2;
    buzzer();
  }

  if (valButtonEnter == 0 && numPage == 1 && cursorPos == 15) // Blue High and Low
  {
    delay(200);
    cursorPos = 0;
    numPage = 3;
    buzzer();
  }

  if (valButtonDown == 0 && numPage == 2)
  {
    delay(200);
    cursorPos = cursorPos + 15;
    buzzer();
    if (cursorPos > 15)
    {
      cursorPos = 0;
    }
  }

  if (valButtonUp == 0 && numPage == 2)
  {
    delay(200);
    cursorPos = cursorPos - 15;
    buzzer();
    if (cursorPos < 0)
    {
      cursorPos = 15;
    }
  }

  if (valButtonDown == 0 && numPage == 3)
  {
    delay(200);
    cursorPos = cursorPos + 15;
    buzzer();
    if (cursorPos > 15)
    {
      cursorPos = 0;
    }
  }

  if (valButtonUp == 0 && numPage == 3)
  {
    delay(200);
    cursorPos = cursorPos - 15;
    buzzer();
    if (cursorPos < 0)
    {
      cursorPos = 15;
    }
  }

  display.setCursor(0, cursorPos);
  display.print(">");

  display.display();
}

void buzzer()
{
  tone(pinBuzzer, 2000);
  delay(50);
  noTone(pinBuzzer);
  delay(50);
}

