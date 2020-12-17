#include <ESP8266WiFi.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "credentials.h"


#define OLED_RESET 4 // not used / nicht genutzt bei diesem Display
Adafruit_SSD1306 display(OLED_RESET);

const char ssid[] = WIFI_SSID_EV;
const char password[] = WIFI_PASSWORD_EV;

WiFiClient client;
   
void setup() {
  Wire.begin(0,2);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.cp437(true); 
  display.setCursor(0, 0);
  display.print("WiFi connected"); 
   display.setCursor(0, 20); 
  display.print("IP address: ");
   display.setCursor(0, 40);
  display.print(WiFi.localIP()); 
  display.display(); 
}
