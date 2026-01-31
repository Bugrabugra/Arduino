#include <Arduino.h>
#include <WiFi.h>

#define TAPO_DEBUG_MODE // Comment this line to disable debug messages
#include "tapo_device.h"

TapoDevice tapo;

constexpr int buttonOffPin = 4;
constexpr int buttonOnPin = 5;


constexpr unsigned long debounceDelay = 50;     // ms
constexpr unsigned long actionCooldownMs = 300; // common duration between two command

// OFF button state
unsigned long lastDebounceTimeOff = 0;
int lastReadingOff = HIGH;
int buttonStateOff = HIGH;
int lastStableStateOff = HIGH;

// ON button state
unsigned long lastDebounceTimeOn = 0;
int lastReadingOn = HIGH;
int buttonStateOn = HIGH;
int lastStableStateOn = HIGH;

// conflict protection
unsigned long lastActionAt = 0;

void setup() {
  Serial.begin(115200);

  pinMode(buttonOffPin, INPUT_PULLUP);
  pinMode(buttonOnPin, INPUT_PULLUP);

  // Connect to Wi-Fi
  WiFi.begin("SONRASI_YOKTU", "BuuRA03045025");

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Initialize Tapo device
  tapo.begin("192.168.1.170", "bugraotken@gmail.com", "vekhym-9subSa-gesput");
  // Example: tapo.begin("192.168.1.100", "abc@example.com", "abc123");

}

void loop() {
  const int readingOff = digitalRead(buttonOffPin);

  if (readingOff != lastReadingOff) {
    lastDebounceTimeOff = millis();
  }

  if ((millis() - lastDebounceTimeOff) > debounceDelay) {
    if (readingOff != buttonStateOff) {
      buttonStateOff = readingOff;

      if (buttonStateOff == LOW && lastStableStateOff == HIGH) {
        if (millis() - lastActionAt >= actionCooldownMs) {
          Serial.println("Button OFF pressed -> Tapo OFF");
          tapo.off();
          lastActionAt = millis();
        }
      }
      lastStableStateOff = buttonStateOff;
    }
  }

  lastReadingOff = readingOff;

  // on button
  const int readingOn = digitalRead(buttonOnPin);

  if (readingOn != lastReadingOn) {
    lastDebounceTimeOn = millis();
  }

  if ((millis() - lastDebounceTimeOn) > debounceDelay) {
    if (readingOn != buttonStateOn) {
      buttonStateOn = readingOn;

      if (buttonStateOn == LOW && lastStableStateOn == HIGH) {
        if (millis() - lastActionAt >= actionCooldownMs) {
          Serial.println("Button ON pressed -> Tapo ON");
          tapo.on();
          lastActionAt = millis();
        }
      }
      lastStableStateOn = buttonStateOn;
    }
  }
  lastReadingOn = readingOn;
}