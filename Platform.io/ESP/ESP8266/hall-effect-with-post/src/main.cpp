#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define ENDPOINT "http://192.168.1.102:3000/hall"

const char *ssid = "SONRASI_YOKTU";
const char *password = "BuuRA03045025";

const int pinHallEffect = 2;
const int pinLed = 3;

String windowState;
int state;

void setup() {
  pinMode(pinHallEffect, INPUT);
  pinMode(pinLed, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  state = digitalRead(pinHallEffect);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, ENDPOINT);  // HTTP
    http.addHeader("Content-Type", "application/json");

    if (digitalRead(pinHallEffect) != state) {
      if (digitalRead(pinHallEffect) == LOW) {
        digitalWrite(pinLed, HIGH);
        windowState = "closed";
        state = 0;
      } else {
        digitalWrite(pinLed, LOW);
        windowState = "open";
        state = 1;
      }

      http.POST("{\"window\":\"salon1\",\"state\": \"" + windowState + "\"  }");
    }
  }

  delay(3000);
}