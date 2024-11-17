#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>

const int pinWifiLed = 0;
const int pinMicroSwitch = 2;

String windowState;
char endpoint[40] = "http://192.168.1.102:3000/hall";
int state;

WiFiManager wm;

void setup() {
  pinMode(pinWifiLed, OUTPUT);
  pinMode(pinMicroSwitch, INPUT);

  // Explicitly set Wi-Fi mode
  WiFi.mode(WIFI_STA);

  bool res;
  res = wm.autoConnect("AutoConnectAP", "password");

  if (res) {
    if (MDNS.begin("esp8266")) {
      digitalWrite(pinWifiLed, HIGH);
    }
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(pinWifiLed, HIGH);

    WiFiClient client;
    HTTPClient http;

    http.begin(client, endpoint);  // HTTP
    http.addHeader("Content-Type", "application/json");

    if (digitalRead(pinMicroSwitch) != state) {
      if (digitalRead(pinMicroSwitch) == LOW) {
        Serial.println("closed");
        windowState = "closed";
        state = 0;
      } else {
        Serial.println("open");
        windowState = "open";
        state = 1;
      }

      http.POST("{\"window\":\"salon1\",\"state\":\"" + windowState + "\"}");
    }

    http.end();
  }

  delay(3000);
}