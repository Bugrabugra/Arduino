#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "credentials.h"


char auth[] = BLYNK_NODEMCU;
char ssid[] = WIFI_SSID_EV;
char pass[] = WIFI_PASSWORD_EV;

void setup() {
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
}