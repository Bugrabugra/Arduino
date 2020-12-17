#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4 // not used / nicht genutzt bei diesem Display
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  Wire.begin(0,2);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    

  Serial.begin(115200);
  delay(10);
}

void loop() {
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.cp437(true); 
  display.setCursor(0, 0);
  display.print("WiFi connected"); 
  display.setCursor(0, 10); 
  display.print("IP address: ");
  display.display(); 
}
