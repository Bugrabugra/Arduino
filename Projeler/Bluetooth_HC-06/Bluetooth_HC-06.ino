#include <SimpleDHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4

String veri = "";
int pinLed = 2;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  pinMode(pinLed, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  veri = "";

  while (Serial.available()) {
    veri += (char)Serial.read();

    if (veri == "1") {
      digitalWrite(pinLed, HIGH);
    } else if (veri == "2") {
      digitalWrite(pinLed, LOW);
    }

    display.clearDisplay();
    display.setTextSize(1.4);
    display.setTextColor(WHITE);
    display.cp437(true);

    display.setCursor(5, 5);
    display.print("Mesaj: ");
    display.setCursor(5, 15);
    display.print(veri);

    display.display();

    delay(100);
  }
}
