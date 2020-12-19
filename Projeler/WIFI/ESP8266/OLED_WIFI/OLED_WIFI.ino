#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
// https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "credentials.h"

SSD1306  display(0x3c, 0, 2);//0x3C being the usual address of the OLED

const char ssid[] = WIFI_SSID_EV;
const char password[] = WIFI_PASSWORD_EV;

WiFiClient client;
   
void setup() {
  Wire.pins(0, 2);// yes, see text
  Wire.begin(0, 2);// 0=sda, 2=scl

  // Initialise the display.
  display.init();

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Connecting to");
  display.drawString(0, 14, ssid);

  // write the buffer to the display
  display.display();
  delay(2000);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.clear();
    display.drawString(0, 0, "Connecting");
    display.display();
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connected");
  display.display();
}

void loop() {

}
