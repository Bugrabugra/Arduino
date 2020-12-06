/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include "credentials.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
const char auth[] = BLYNK_PASS;

// Your WiFi credentials.
// Set password to "" for open networks.

// const char ssid[] = WIFI_SSID_EV;
// const char pass[] = WIFI_PASSWORD_EV;
const char ssid[] = WIFI_SSID_EV;
const char pass[] = WIFI_PASSWORD_EV;

int pinIR = 5;

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(pinIR, INPUT_PULLUP);
}

void loop()
{
  Blynk.run();

  if (!digitalRead(pinIR))
  {
    Blynk.notify("Pülü tuvalette!");
  }
}
