#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button pins
int pinButtonUp = 2;
int pinButtonDown = 3;
int pinButtonBack = 4;
int pinButtonEnter = 5;

// LED pins
int pinLEDGreen = 6;
int pinLEDBlue = 7;

// Buzzer pin
int pinBuzzer = 8;

// Button values
int valButtonDown = 0;
int valButtonUp = 0;
int valButtonEnter = 0;
int valButtonBack = 0;

// Other constant and variables
int cursorPos = 0;
int numPage = 0;

int pixelsPerRow = 15;
int menuRowCount = 0;

// Menu names
char *menuMain[] = { "Araclar", "Uygulamalar", "Haritalar", "LEDler" };
char *menuLEDler[] = { "Green LED", "Blue LED" };
char *menuLEDGreen[] = { "Green High", "Green Low" };
char *menuLEDBlue[] = { "Blue High", "Blue Low" };


void setup() {
  // Start display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Pin modes declaration
  pinMode(pinButtonDown, INPUT_PULLUP);
  pinMode(pinButtonUp, INPUT_PULLUP);
  pinMode(pinButtonEnter, INPUT_PULLUP);
  pinMode(pinButtonBack, INPUT_PULLUP);
  pinMode(pinLEDGreen, OUTPUT);
  pinMode(pinLEDBlue, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
}

void loop() {
  // Button values
  valButtonDown = digitalRead(pinButtonDown);
  valButtonUp = digitalRead(pinButtonUp);
  valButtonEnter = digitalRead(pinButtonEnter);
  valButtonBack = digitalRead(pinButtonBack);

  // Check page number
  switch (numPage) {
    case 0:
      menu_populator(menuMain);
      menuRowCount = 4;
      break;
    case 1:
      menu_populator(menuLEDler);
      menuRowCount = 2;
      break;
    case 2:
      menu_populator(menuLEDGreen);
      menuRowCount = 2;
      break;
    case 3:
      menu_populator(menuLEDBlue);
      menuRowCount = 2;
      break;
  }

  //Create menus
  menu_creator();
}

// Buzzer
void buzzer(bool menu) {
  if (menu == true) {
    tone(pinBuzzer, 2000);
    delay(50);
    noTone(pinBuzzer);
    delay(50);
  } else {
    tone(pinBuzzer, 2000);
    delay(50);
    noTone(pinBuzzer);
    delay(30);
    tone(pinBuzzer, 2000);
    delay(30);
    noTone(pinBuzzer);
    delay(30);
  }
}

void menu_populator(char *menu_name_list[]) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.cp437(true);

  // Populating menu items to screen
  for (int i = 0; i < menuRowCount; i++) {
    display.setCursor(10, pixelsPerRow * i);
    display.print(menu_name_list[i]);
  }

  // Menu walker buy up and down buttons
  if (valButtonDown == 0) {
    delay(200);
    cursorPos = cursorPos + pixelsPerRow;
    buzzer(true);
    if (cursorPos > ((menuRowCount - 1) * pixelsPerRow)) {
      cursorPos = 0;
    }
  }

  if (valButtonUp == 0) {
    delay(200);
    cursorPos = cursorPos - pixelsPerRow;
    buzzer(true);
    if (cursorPos < 0) {
      cursorPos = ((menuRowCount - 1) * pixelsPerRow);
    }
  }

  // Placing cursor symbol
  display.setCursor(0, cursorPos);
  display.print(">");

  // Showing items on screen
  display.display();
}

void menu_creator() {
  // Enter button
  if (valButtonEnter == 0) {
    // LED's menu
    if (numPage == 0 && cursorPos == 45) {
      delay(200);
      numPage = 1;
      buzzer(false);
      menu_populator(menuLEDler);
      cursorPos = 0;
    }
    // Green LED menu
    else if (numPage == 1 && cursorPos == 0) {
      delay(200);
      numPage = 2;
      buzzer(false);
      menu_populator(menuLEDGreen);
      cursorPos = 0;
    }
    // Green LED menu, Green High
    else if (numPage == 2 && cursorPos == 0) {
      delay(200);
      buzzer(false);
      digitalWrite(pinLEDGreen, HIGH);
    }
    // Green LED menu, Green Low
    else if (numPage == 2 && cursorPos == 15) {
      delay(200);
      buzzer(false);
      digitalWrite(pinLEDGreen, LOW);
    }
    // Blue LED menu
    else if (numPage == 1 && cursorPos == 15) {
      delay(200);
      numPage = 3;
      buzzer(false);
      menu_populator(menuLEDBlue);
      cursorPos = 0;
    }
    // Blue LED menu, Blue High
    else if (numPage == 3 && cursorPos == 0) {
      delay(200);
      buzzer(false);
      digitalWrite(pinLEDBlue, HIGH);
    }
    // Blue LED menu, Blue Low
    else if (numPage == 3 && cursorPos == 15) {
      delay(200);
      buzzer(false);
      digitalWrite(pinLEDBlue, LOW);
    }
  }

  // Back button
  if (valButtonBack == 0) {
    // While in LED's menu
    if (numPage == 1) {
      delay(200);
      numPage = 0;
      buzzer(true);
      cursorPos = 0;
    }
    // While in Green LED menu
    else if (numPage == 2) {
      delay(200);
      numPage = 1;
      buzzer(true);
      cursorPos = 0;
    }
    // While in Blue LED menu
    else if (numPage == 3) {
      delay(200);
      numPage = 1;
      buzzer(true);
      cursorPos = 0;
    }
  }
}
