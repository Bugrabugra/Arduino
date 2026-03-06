#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>

#define TAPO_DEBUG_MODE
#include "tapo_device.h"

// --- Display ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Buttons ---
#define BTN_UP 2
#define BTN_DOWN 3
#define BTN_BACK 5
#define BTN_ENTER 4

// --- WiFi ---
const char *WIFI_SSID = "SONRASI_YOKTU";
const char *WIFI_PASS = "BuuRA03045025";

// --- Tapo Credentials ---
const char *TAPO_USER = "bugraotken@gmail.com";
const char *TAPO_PASS = "vekhym-9subSa-gesput";

// --- Devices ---
const int DEVICE_COUNT = 3;

struct DeviceInfo {
  const char *name;
  const char *ip;
  bool statusKnown;
  bool isOn;
};

DeviceInfo devices[DEVICE_COUNT] = {
  {"Bilgisayar Odasi", "192.168.1.170", false, false},
  {"Salon TV Yani", "192.168.1.173", false, false},
  {"Yatak Odasi", "192.168.1.172", false, false},
};

TapoDevice tapo[DEVICE_COUNT];

// --- State ---
int menuIndex = 0;

// --- Display Helpers ---
void showMessage(const char *title, const char *body = nullptr) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.println(title);
  if (body) {
    display.setCursor(0, 32);

    display.println(body);
  }
  display.display();
}

void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Header
  display.setCursor(0, 0);
  display.print("  TAPO CONTROL");
  display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

  // Device list: 3 items, each 17px tall, starting at y=11
  // Layout per item: name at y+1, status at y+9
  for (int i = 0; i < DEVICE_COUNT; i++) {
    int y = 11 + i * 17;
    bool selected = (menuIndex == i);

    if (selected) {
      display.fillRect(0, y, 127, 16, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(2, y + 1);
    display.print(devices[i].name);

    display.setCursor(2, y + 9);
    if (!devices[i].statusKnown) {
      display.print("Status: ?");
    } else {
      display.print(devices[i].isOn ? "Status: ON" : "Status: OFF");
    }

    display.setTextColor(SSD1306_WHITE);
  }

  display.display();
}

// --- Setup ---
void setup() {
  Serial.begin(115200);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_ENTER, INPUT_PULLUP);

  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }

  showMessage("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  char ipStr[20];
  WiFi.localIP().toString().toCharArray(ipStr, sizeof(ipStr));
  showMessage("WiFi connected!", ipStr);
  delay(1000);

  // Handshake and fetch initial status for each device
  for (int i = 0; i < DEVICE_COUNT; i++) {
    char msg[32];
    snprintf(msg, sizeof(msg), "Connecting %d/%d", i + 1, DEVICE_COUNT);
    showMessage(msg, devices[i].name);
    tapo[i].begin(devices[i].ip, TAPO_USER, TAPO_PASS);
    devices[i].isOn = tapo[i].isOn();
    devices[i].statusKnown = true;
  }

  drawMenu();
}

// --- Loop ---
void loop() {
  if (digitalRead(BTN_UP) == LOW) {
    delay(50);
    if (digitalRead(BTN_UP) == LOW) {
      menuIndex--;
      if (menuIndex < 0)
        menuIndex = DEVICE_COUNT - 1;
      drawMenu();
      while (digitalRead(BTN_UP) == LOW)
        ;
    }
  }

  if (digitalRead(BTN_DOWN) == LOW) {
    delay(50);
    if (digitalRead(BTN_DOWN) == LOW) {
      menuIndex++;
      if (menuIndex >= DEVICE_COUNT)
        menuIndex = 0;
      drawMenu();
      while (digitalRead(BTN_DOWN) == LOW)
        ;
    }
  }

  if (digitalRead(BTN_ENTER) == LOW) {
    delay(50);
    if (digitalRead(BTN_ENTER) == LOW) {
      while (digitalRead(BTN_ENTER) == LOW)
        ;

      const char *action = devices[menuIndex].isOn ? "Turning off..." : "Turning on...";
      showMessage(action, devices[menuIndex].name);

      if (devices[menuIndex].isOn) {
        tapo[menuIndex].off();
      } else {
        tapo[menuIndex].on();
      }

      // Re-fetch status to confirm
      devices[menuIndex].isOn = tapo[menuIndex].isOn();
      devices[menuIndex].statusKnown = true;

      drawMenu();
    }
  }

  // BACK: refresh all device statuses
  if (digitalRead(BTN_BACK) == LOW) {
    delay(50);
    if (digitalRead(BTN_BACK) == LOW) {
      while (digitalRead(BTN_BACK) == LOW)
        ;
      showMessage("Refreshing statuses...");
      for (int i = 0; i < DEVICE_COUNT; i++) {
        devices[i].isOn = tapo[i].isOn();
        devices[i].statusKnown = true;
      }
      drawMenu();
    }
  }
}
