#define BLYNK_PRINT Serial

#include "credentials.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_PASS;
char ssid[] = WIFI_SSID_EV;
char pass[] = WIFI_PASSWORD_EV;

int pinIR = 3;

void setup() {
  // Debug console
  Serial.begin(9600);
  pinMode(pinIR, INPUT);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  if (analogRead(pinIR) < 800) {
    Blynk.virtualWrite(V5, analogRead(pinIR));
  }
}
