#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Button D3 -> GPIO5
#define BUTTON_PIN 5

void setup() {
  pinMode(BUTTON_PIN, INPUT);

  // Start OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  const int state = digitalRead(BUTTON_PIN);

  if (state == LOW) {
    esp_deep_sleep_enable_gpio_wakeup(1ULL << BUTTON_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);
  } else {
    esp_deep_sleep_enable_gpio_wakeup(1ULL << BUTTON_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);
  }

  // Attach wakeup pin
  // esp_deep_sleep_enable_gpio_wakeup(1ULL << BUTTON_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);

  // 5 seconds countdown
  for (int i = 5; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Sleeping in: ");
    display.print(i);
    display.print(" sec");
    display.display();
    delay(1000);
  }

  // Sleeping message
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Sleeping");
  display.setCursor(0, 40);
  display.println(state == LOW ? "At home" : "Not home");
  display.display();
  delay(500);

  // Deep sleep
  esp_deep_sleep_start();
}

void loop() {
}
