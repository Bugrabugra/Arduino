#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define ENDPOINT "http://192.168.1.101:3000/hall"

const char *ssid = "SONRASI_YOKTU";
const char *password = "BuuRA03045025";

const int pinHallEffect = 2;
int currentHallEffect;

void setup() {
  Serial.begin(115200);

  pinMode(pinHallEffect, INPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  currentHallEffect = digitalRead(pinHallEffect);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    http.begin(client, ENDPOINT);  // HTTP
    http.addHeader("Content-Type", "application/json");

    int httpCode;

    Serial.print("[HTTP] POST...\n");

    Serial.println("Current hall effect value: ");
    Serial.println(currentHallEffect);

    Serial.println("Digital read: ");
    Serial.println(digitalRead(pinHallEffect));

    if (currentHallEffect != digitalRead(pinHallEffect)) {
      if (digitalRead(pinHallEffect) == 0) {
        currentHallEffect = 0;
        httpCode = http.POST(R"({"window":"salon1","state":"close"})");
      } else {
        currentHallEffect = 1;
        httpCode = http.POST(R"({"window":"salon1","state":"open"})");
      }

      Serial.println(httpCode);

      if (httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK) {
          const String &payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", HTTPClient::errorToString(httpCode).c_str());
      }

      http.end();
    }
  }

  delay(3000);
}
