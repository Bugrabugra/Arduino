#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include "credentials.h"

//----------------  Fill in your credentails   ---------------------
const char ssid[] = WIFI_SSID_EV;
const char pass[] = WIFI_PASSWORD_EV;
//const char ssid[] = WIFI_SSID_IS;
//const char pass[] = WIFI_PASSWORD_IS;
//------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_BUILTIN, OUTPUT);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(WIFI_SSID_EV, WIFI_PASSWORD_EV);

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
}
