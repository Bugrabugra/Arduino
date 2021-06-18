// Adafruit_ST77xx.h dosyasında #define ST77XX_MADCTL_MY 0 olacak!

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
 
//Arduino Uno/Nano Pins:
//GND = GND
//VCC = 5v
//DC = 8
//RES = 9
//CS = 10
//SDA = 11
//SCL = 13
 
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  tft.initR(INITR_MINI160x80);
 
  tft.invertDisplay(true);

  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
 
  tft.setCursor(5, 5);
  tft.setTextColor(ST7735_RED); 
  tft.setTextSize(1);
  tft.println("Welcome to");
  tft.setCursor(5, 20);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2);
  tft.println("ezContents");
  tft.setCursor(5, 38);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(3);
  tft.print("Blogs");
 
  tft.drawLine(153, 0, 153, 79, ST7735_WHITE);
  tft.drawCircle(153, 20, 5, ST7735_CYAN);
  tft.fillTriangle(153, 35, 148, 45, 158, 45, ST7735_MAGENTA);
  tft.drawTriangle(153, 35, 148, 45, 158, 45, ST7735_WHITE);
  tft.fillCircle(153, 60, 5, ST7735_YELLOW);
}
 
void loop()
{
  for (int i=0; i < 100; i++) {
    tft.setCursor(5, 5);
    tft.setTextColor(ST7735_ORANGE);
    tft.setTextSize(10);
    tft.println(i);
    delay(1000);
    tft.fillScreen(ST7735_BLACK);
  }
}