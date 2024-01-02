// Hardware setup:
// Attach a rotary encoder with output pins to
// * A2 and A3 on Arduino UNO.
// * D5 and D6 on ESP8266 board (e.g. NodeMCU).
// Swap the pins when direction is detected wrong.
// The common contact should be attached to ground.

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library
#include <SPI.h>

//#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input signals on pin 2 and 3
//#define PIN_IN1 6
//#define PIN_IN2 7

//#elif defined(ESP8266)
// Example for ESP8266 NodeMCU with input signals on pin D5 and D6
//#define PIN_IN1 D5
//#define PIN_IN2 D6

//#endif

#define PIN_IN1 6
#define PIN_IN2 7
#define pinButton 12
#define debounce 300

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

//Arduino Uno/Nano Pins:
//GND = GND
//VCC = 5v
//DC = 8
//RES = 9
//CS = 10
//SDA = 11
//SCL = 13

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  pinMode(pinButton, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");

  tft.initR(INITR_MINI160x80);

  tft.invertDisplay(true);

  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(8);
}


// Read the current position of the encoder and print out when changed.
void loop() {
  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    if (newPos % 2 == 0) {
      Serial.print("pos:");
      Serial.print(newPos / 2);
      Serial.print(" dir:");
      Serial.println((int)(encoder.getDirection()));

      if (newPos / 2 % 2 == 0) {
        tft.setCursor(5, 5);
        tft.setTextColor(ST7735_ORANGE, ST7735_BLACK);
        tft.print(newPos / 2);
      } else {
        tft.setCursor(5, 5);
        tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
        tft.print(newPos / 2);
      }
    }

    pos = newPos;
  }

  if (digitalRead(pinButton) == 0) {
    tft.setCursor(5, 5);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_GREEN);
    tft.print("X");
    Serial.println("button");
    delay(debounce);
  }
}
