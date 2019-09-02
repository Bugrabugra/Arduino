#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#include "oled_buf.h"      //your bitmap
#include "tractor_128x64.h"


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();  
  delay(1000);
}

void loop() 
{
    display.clearDisplay();
    // your bitmap is organised for each byte as 8-bit vertical stripe per column
    for (int row = 0; row < 64; row += 8) {
        for (int col = 0; col < 128; col++) {
            uint8_t c = pgm_read_byte(oled_buf + (row / 8) * 128 + col);
            for (uint8_t cnt = 0; cnt < 8; cnt++) {
                display.drawPixel(col, row + cnt, (c & 0x01) ? WHITE : BLACK);
                c >>= 1;
            }
        }
    }
    display.display();
    delay(1000);
    // this shows a regular bitmap i.e. horiz stripes
    display.drawBitmap(0, 0, tractor_128x64, 128, 64, BLACK, WHITE);
    display.display();
    delay(1000);
}
