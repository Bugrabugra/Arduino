#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define BTN_UP 2
#define BTN_DOWN 3
#define BTN_BACK 5
#define BTN_ENTER 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum MenuState {
  MAIN_MENU,
  SETTINGS_MENU,
  INFO_MENU,
  ABOUT_MENU
};

MenuState currentMenu = MAIN_MENU;
int menuIndex = 0;
int maxMenuItems = 3;

void handleButtons();
void enterMenu();
void goBack();
void updateDisplay();
void displayMainMenu();
void displaySettingsMenu();
void displayInfoMenu();
void displayAboutMenu();

void setup() {
  Serial.begin(115200);
  
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_ENTER, INPUT_PULLUP);
  
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("OLED Menu System");
  display.println("Ready!");
  display.display();
  delay(2000);
}

void loop() {
  handleButtons();
  updateDisplay();
  delay(100);
}

void handleButtons() {
  if (digitalRead(BTN_UP) == LOW) {
    delay(50);
    if (digitalRead(BTN_UP) == LOW) {
      menuIndex--;
      if (menuIndex < 0) menuIndex = maxMenuItems - 1;
      while(digitalRead(BTN_UP) == LOW);
    }
  }
  
  if (digitalRead(BTN_DOWN) == LOW) {
    delay(50);
    if (digitalRead(BTN_DOWN) == LOW) {
      menuIndex++;
      if (menuIndex >= maxMenuItems) menuIndex = 0;
      while(digitalRead(BTN_DOWN) == LOW);
    }
  }
  
  if (digitalRead(BTN_ENTER) == LOW) {
    delay(50);
    if (digitalRead(BTN_ENTER) == LOW) {
      enterMenu();
      while(digitalRead(BTN_ENTER) == LOW);
    }
  }
  
  if (digitalRead(BTN_BACK) == LOW) {
    delay(50);
    if (digitalRead(BTN_BACK) == LOW) {
      goBack();
      while(digitalRead(BTN_BACK) == LOW);
    }
  }
}

void enterMenu() {
  switch(currentMenu) {
    case MAIN_MENU:
      switch(menuIndex) {
        case 0: currentMenu = SETTINGS_MENU; menuIndex = 0; maxMenuItems = 3; break;
        case 1: currentMenu = INFO_MENU; menuIndex = 0; maxMenuItems = 2; break;
        case 2: currentMenu = ABOUT_MENU; menuIndex = 0; maxMenuItems = 1; break;
      }
      break;
    case SETTINGS_MENU:
      switch(menuIndex) {
        case 0: break;
        case 1: break;
        case 2: goBack(); break;
      }
      break;
    case INFO_MENU:
    case ABOUT_MENU:
      goBack();
      break;
  }
}

void goBack() {
  switch(currentMenu) {
    case SETTINGS_MENU:
    case INFO_MENU:
    case ABOUT_MENU:
      currentMenu = MAIN_MENU;
      menuIndex = 0;
      maxMenuItems = 3;
      break;
    default:
      break;
  }
}

void updateDisplay() {
  display.clearDisplay();
  
  switch(currentMenu) {
    case MAIN_MENU:
      displayMainMenu();
      break;
    case SETTINGS_MENU:
      displaySettingsMenu();
      break;
    case INFO_MENU:
      displayInfoMenu();
      break;
    case ABOUT_MENU:
      displayAboutMenu();
      break;
  }
  
  display.display();
}

void displayMainMenu() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("=== MAIN MENU ===");
  display.println("");
  
  display.println(menuIndex == 0 ? "> Settings" : "  Settings");
  display.println(menuIndex == 1 ? "> Info" : "  Info");
  display.println(menuIndex == 2 ? "> About" : "  About");
  
  display.setCursor(0,55);
}

void displaySettingsMenu() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("=== SETTINGS ===");
  display.println("");
  
  display.println(menuIndex == 0 ? "> Brightness: 100%" : "  Brightness: 100%");
  display.println(menuIndex == 1 ? "> Sound: ON" : "  Sound: ON");
  display.println(menuIndex == 2 ? "> Back" : "  Back");
  
  display.setCursor(0,55);
  display.println("BACK to return");
}

void displayInfoMenu() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("=== INFO ===");
  display.println("");
  
  display.println(menuIndex == 0 ? "> System Status" : "  System Status");
  display.println(menuIndex == 1 ? "> Back" : "  Back");
  
  display.setCursor(0,55);
  display.println("BACK to return");
}

void displayAboutMenu() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("=== ABOUT ===");
  display.println("");
  display.println("OLED Menu System");
  display.println("Version 1.0");
  display.println("SSD1315 Display");
  display.println("4 Button Control");
  display.println("");
  display.println("Press any key");
}