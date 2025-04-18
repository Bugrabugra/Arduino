#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned char Untitled[] PROGMEM = {
    0x00, 0xFC, 0x00, 0x00, 0x3E, 0x00, 0x01, 0xFF, 0x80, 0x01, 0xFF, 0x80, 0x07, 0xFF, 0xE0, 0x07,
    0xFF, 0xE0, 0x0F, 0x07, 0xF0, 0x0F, 0xC1, 0xF0, 0x1F, 0x00, 0xF0, 0x0F, 0x00, 0xF8, 0x38, 0x00,
    0x78, 0x1C, 0x00, 0x3C, 0x38, 0x00, 0x3C, 0x3C, 0x00, 0x1C, 0x78, 0x00, 0x1E, 0x38, 0x00, 0x1E,
    0x70, 0x00, 0x0E, 0x70, 0x00, 0x0E, 0xE0, 0x40, 0x0F, 0xF0, 0x02, 0x0F, 0xF1, 0xE0, 0x0F, 0xF8,
    0x07, 0x87, 0xE7, 0xC0, 0xFF, 0xFF, 0x03, 0xE7, 0xEF, 0xC1, 0xFC, 0x3F, 0x83, 0xF7, 0xFF, 0xC3,
    0xF0, 0x0F, 0xC3, 0xFF, 0xFF, 0x83, 0xC0, 0x03, 0xE1, 0xFF, 0x7B, 0x0F, 0x00, 0x00, 0xF8, 0xDF,
    0x7B, 0x9E, 0x00, 0x00, 0x78, 0xDE, 0x7B, 0x3C, 0x00, 0x00, 0x3C, 0xCE, 0x03, 0x78, 0x00, 0x00,
    0x3E, 0xC0, 0x03, 0x78, 0x00, 0x00, 0x1E, 0xC0, 0x03, 0xF0, 0x00, 0x00, 0x0F, 0xC0, 0x03, 0xE0,
    0x01, 0x80, 0x07, 0x80, 0x01, 0xC0, 0x03, 0xC0, 0x03, 0x80, 0x01, 0xC0, 0x0F, 0xF0, 0x03, 0x00,
    0x01, 0x83, 0x8E, 0x71, 0xC1, 0x80, 0x01, 0x83, 0x8E, 0x71, 0xC1, 0x80, 0x01, 0x81, 0x8E, 0x79,
    0x81, 0x80, 0x01, 0x00, 0x1C, 0x38, 0x01, 0x80, 0x03, 0x80, 0x1C, 0x38, 0x00, 0xC0, 0x03, 0x00,
    0x1C, 0x38, 0x00, 0xC0, 0x03, 0x00, 0x1C, 0x1C, 0x00, 0xC0, 0x03, 0x00, 0x1C, 0x38, 0x00, 0xC0,
    0x03, 0x00, 0x1E, 0x78, 0x00, 0xC0, 0x03, 0x80, 0x0F, 0xF0, 0x00, 0xC0, 0x01, 0x00, 0x0F, 0xF0,
    0x01, 0x80, 0x03, 0x80, 0x03, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0xC0,
    0x00, 0x00, 0x03, 0x80, 0x00, 0xC0, 0x34, 0x1E, 0x03, 0x00, 0x00, 0xE0, 0x7F, 0xFE, 0x07, 0x00,
    0x00, 0xF0, 0x3F, 0xFC, 0x0F, 0x00, 0x00, 0x7C, 0x07, 0xE0, 0x3E, 0x00, 0x00, 0x3C, 0x00, 0x00,
    0x3E, 0x00, 0x00, 0x1F, 0xC0, 0x03, 0xF8, 0x00, 0x00, 0x0F, 0xF8, 0x2F, 0xF0, 0x00, 0x00, 0x03,
    0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00
};

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  display.clearDisplay();
  display.drawBitmap(40, 17, Untitled, 48, 48, 1);
  display.setTextSize(1);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.cp437(true);          // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 0);
  display.print("Curukler & Bedelliler");

  display.display();
}
