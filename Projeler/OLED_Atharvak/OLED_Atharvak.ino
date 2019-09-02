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

#include "untitled.h"      //your bitmap
#include "tractor_128x64.h"

// your weird format bitmap is similar to KS0108 GLCD displays
void drawKS0108map_P(int x, int y, const uint8_t *buf, int w, int h)
{
    for (int row = 0; row < h; row += 8) {
        for (int col = 0; col < w; col++) {
            uint8_t c = pgm_read_byte(buf + (row / 8) * 128 + col);
            for (uint8_t cnt = 0; cnt < 8; cnt++) {
                display.drawPixel(x + col, y + row + cnt, (c & 0x01) ? WHITE : BLACK);
                c >>= 1;
            }
        }
    }
}

bool error(char *msg1, char *msg2)
{
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.clearDisplay();
    display.print(msg1);
    display.print(msg2);
    display.display();
    delay(5000);
    return false;
}

bool write_buf_SD(char *name)
{
    File f = SD.open(name, FILE_WRITE);
    bool good = (f != NULL);
    if (!good) return error("can't open", name);
    if (f.size() >= 1024) error(name, " exists");
    else f.write(display.getBuffer(), 1024);
    f.close();
    return good;
}

bool read_buf_SD(char *name)
{
    File f = SD.open(name, FILE_READ);
    bool good = (f != NULL);
    if (good) {
        f.read(display.getBuffer(), 1024);
        f.close();
    }
    else return error("can not find ", name);
    display.display();
    delay(1000);
    return good;
}

void setup()
{

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // required to run SSD1306
    display.display();                          // show Adafruit logo
    delay(1000);
    bool good = SD.begin(SD_CS);
    if (!good) {
        error("cannot start SD", "\nstopping here");
        while (1);                              //tread water
    }
    // draw YOUR bitmap in Flash to the internal buffer
    drawKS0108map_P(0, 0, oled_buf, 128, 64);
    display.display();
    // write the buffer to file on SD
    write_buf_SD("oled_buf.bin");
    // same thing but this is a regular format bitmap
    display.drawBitmap(0, 0, tractor_128x64, 128, 64, BLACK, WHITE);
    display.display();
    write_buf_SD("tractor.bin");
}

void loop()
{
    /*
        display.clearDisplay();
        // your bitmap is organised for each byte as 8-bit vertical stripe per column
        drawKS0108map_P(0, 0, oled_buf, 128, 64);
        display.display();
        delay(1000);
    */
    // this shows a regular bitmap from Flash but I have swapped WHITE/BLACK
    display.drawBitmap(0, 0, tractor_128x64, 128, 64, WHITE, BLACK);
    display.display();
    delay(1000);
    // reads and displays bitmap from SD
    read_buf_SD("oled_buf.bin");
    read_buf_SD("tractor.bin");
}
