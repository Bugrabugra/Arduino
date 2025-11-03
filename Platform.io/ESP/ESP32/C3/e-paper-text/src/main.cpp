#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <SPI.h>
#include "image.h" // your bitmap logo

// Pin mapping
#define EPD_CS   D1
#define EPD_DC   D3
#define EPD_RST  D0
#define EPD_BUSY D5

// Display for GDEY029T94 panel
GxEPD2_BW<GxEPD2_290_GDEY029T94, GxEPD2_290_GDEY029T94::HEIGHT> display(GxEPD2_290_GDEY029T94(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

float fakeTemperature = 25.0;

void setup() {
  SPI.begin();
  display.init(115200);
  display.setRotation(3);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setFullWindow();
  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, image, 296, 128, GxEPD_WHITE, GxEPD_BLACK);

    display.setCursor(80, 10);
    display.print("Pulusum");

    display.setCursor(80, 30);
    display.print("=))))");

    display.setCursor(80, 60);
    display.print(fakeTemperature, 1);
  } while (display.nextPage());
}

void loop() {
  fakeTemperature += 0.1;

  display.setPartialWindow(80, 50, 50, 20);
  display.firstPage();

  do {
    // display.fillRect(80, 50, 50, 20, GxEPD_WHITE);
    display.setCursor(80, 60);
    display.print(fakeTemperature, 1);
  } while (display.nextPage());

  delay(2000);
}