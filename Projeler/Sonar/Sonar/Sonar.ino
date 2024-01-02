//SCL = A5
//SDA = A4

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char* kelimeler[] = { "CANIM", "ANNEM", "SENI", "COK", "SEVIYORUM", "=)" };

void setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Address 0x3D for 128x64

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  display.setTextSize(2);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.cp437(true);          // Use full 256 char 'Code Page 437' font

  int index = 0;
  while (index < 6) {
    delay(300);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(kelimeler[index]);
    display.display();
    delay(500);
    index = index + 1;
  }
}
