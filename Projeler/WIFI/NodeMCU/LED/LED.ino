#include <ESP8266WiFi.h>
#include "credentials.h"

//----------------  Fill in your credentails   ---------------------
const char ssid[] = WIFI_SSID_EV;
const char pass[] = WIFI_PASSWORD_EV;
//const char ssid[] = WIFI_SSID_IS;
//const char pass[] = WIFI_PASSWORD_IS;
//------------------------------------------------------------------

int pinLED = 5;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  pinMode(pinLED, OUTPUT);

  digitalWrite(pinLED, HIGH);
}

void loop() {
}
