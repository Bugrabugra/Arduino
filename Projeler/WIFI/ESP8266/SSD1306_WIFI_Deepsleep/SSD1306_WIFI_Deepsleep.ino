#include <Wire.h> // Only needed for Arduino 1.6.5 and earlier
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "credentials.h"


SSD1306 display(0x3c, 0, 2);

const char ssid[] = WIFI_SSID_EV;
const char password[] = WIFI_PASSWORD_EV;
const char *host = "https://timeapi.io/api/Time/current/zone?timeZone=Turkey"; //We read the data from this host
int port = 443;

String Data_Raw;
WiFiClientSecure client;
HTTPClient http;

void setup() {
  display.displayOn();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.drawString(33, 33, "Connecting to");
  display.drawString(20, 42, ssid);
  display.display();
  WiFi.begin(ssid, password);
}
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(35, 33, "Connected!");
    display.drawString(20, 42, "IP: " + WiFi.localIP().toString());
    display.display();
  }

  client.setInsecure(); //the magic line, use with caution
  client.connect(host, port);
  // client.setFingerprint(fingerprint);
  http.begin(client, host);
  // https.addHeader("Content-Type", "application/json");
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      Data_Raw = http.getString();
    }
    
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, Data_Raw);
    const char *time = doc["time"]; // "01:50"
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Result: ");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 15, time);
    display.display();
    http.end();
    delay(5000);
    display.displayOff();
    ESP.deepSleep(15 * 1000000);
  }
}