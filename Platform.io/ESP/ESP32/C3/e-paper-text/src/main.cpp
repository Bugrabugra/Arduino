#include <Arduino.h>

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <SPI.h>
#include "image.h"

// Pin mapping
#define EPD_CS   D1
#define EPD_DC   D3
#define EPD_RST  D0
#define EPD_BUSY D5

// Use the correct driver for T94 panel (change model if needed)
GxEPD2_BW<GxEPD2_290_GDEY029T94, GxEPD2_290_GDEY029T94::HEIGHT> display(GxEPD2_290_GDEY029T94(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

void setup() {
  Serial.begin(115200);
  SPI.begin();

  display.init(115200);       // Initialize the ePaper
  display.setRotation(3);     // Adjust orientation if needed
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setFullWindow();    // Full screen refresh
  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, image, 296, 128, GxEPD_WHITE, GxEPD_BLACK);
    display.setCursor(80, 10);
    display.print("Pulusum");

    display.setCursor(80, 30);
    display.print("=))))");

  } while (display.nextPage());

  Serial.println("Display done!");
}

void loop() {}