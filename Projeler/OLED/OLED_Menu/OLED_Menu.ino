#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int pinButtonDown = 7;
int pinButtonUp = 6;
int pinButtonEnter = 5;

int valButtonDown = 0;
int valButtonUp = 0;
int valButtonEnter = 0;

int cursorPos = 0;


void setup()
{
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(pinButtonDown, INPUT_PULLUP);
  pinMode(pinButtonUp, INPUT_PULLUP);
  pinMode(pinButtonEnter, INPUT_PULLUP);
}

void loop()
{
  valButtonDown = digitalRead(pinButtonDown);
  valButtonUp = digitalRead(pinButtonUp);
  valButtonEnter = digitalRead(pinButtonEnter);

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
  display.print("Mesajlar");



  if (valButtonDown == 0)
  {
    delay(200);
    cursorPos = cursorPos + 15;
    if (cursorPos > 45)
    {
      cursorPos = 0;
    }
  }

  if (valButtonUp == 0)
  {
    delay(200);
    cursorPos = cursorPos - 15;
    if (cursorPos < 0)
    {
      cursorPos = 45;
    }
  }

  display.setCursor(0, cursorPos);
  display.print(">");

  display.display();
}

