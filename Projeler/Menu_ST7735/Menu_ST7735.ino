#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
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
 
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Menu names
char *menuMain[] = {"LED", "Brightness"};
char *menuLED[] = {"On", "Off"};
int numPage = 0;
int menuRowCount = 0;
int pixelsPerRow = 10;
int cursorPos = 20;

void setup() {
  pinMode(pinButton, INPUT_PULLUP);
  Serial.begin(9600);
  tft.initR(INITR_MINI160x80);
  tft.invertDisplay(true);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);
}

void menuTitle(const char* text) {
  tft.setCursor(5, 5);
  tft.setTextColor(ST7735_ORANGE); 
  tft.print(text);
  tft.drawLine(0, 15, 160, 15, ST7735_ORANGE);
}

void menu_populator(char *menu_name_list[]) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_GREEN);

  // Populating menu items to screen
  for (int i = 0; i < menuRowCount; i++) {
    tft.setCursor(10, pixelsPerRow * i);
    tft.print(menu_name_list[i]);
  }

  // Menu walker buy up and down buttons
  if ((int)(encoder.getDirection()) == 1) {
    delay(200);
    cursorPos = cursorPos + pixelsPerRow;
    
    if (cursorPos > ((menuRowCount-1) * pixelsPerRow)) {
      cursorPos = 15;
    }
  }

  if ((int)(encoder.getDirection()) == -1) {
    delay(200);
    cursorPos = cursorPos - pixelsPerRow;
    
    if (cursorPos < 0) {
      cursorPos = ((menuRowCount-1) * pixelsPerRow);
    }
  }

  // Placing cursor symbol
  tft.setCursor(0, cursorPos);
  tft.print(">");
}

void loop() {
  static int pos = 0;
  encoder.tick();

  int chevronPosition = 5;
  int newPos = encoder.getPosition();
  Serial.print(newPos);

  if (pos != newPos && (int)(encoder.getDirection()) == 1) {
    chevronPosition = chevronPosition + 10;
    tft.setCursor(5, chevronPosition);
    tft.print(">");
    pos = newPos;
  } else if (pos != newPos && (int)(encoder.getDirection()) == -1){
    chevronPosition = chevronPosition - 10;
    tft.setCursor(5, chevronPosition);
    tft.print(">");
    pos = newPos;
  }


  // int newPos = encoder.getPosition();
  // if (pos != newPos) {
  //   if (newPos % 2 == 0) {
  //     Serial.print("pos:");
  //     Serial.print(newPos/2);
  //     Serial.print(" dir:");
  //     Serial.println((int)(encoder.getDirection()));
  //   }

  //   pos = newPos;
  // }


    // Check page number
  switch (numPage) {
  case 0:
    menuTitle("Main menu");
    menu_populator(menuMain);
    menuRowCount = 2;
    break;
  case 1:
    menuTitle("LED");
    menu_populator(menuLED);
    menuRowCount = 2;
    break;
  }
}
