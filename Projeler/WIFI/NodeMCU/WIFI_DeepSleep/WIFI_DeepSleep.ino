#include <ESP8266WiFi.h>
#include "credentials.h"

//----------------  Fill in your credentails   ---------------------
const char ssid[] = WIFI_SSID_EV;
const char pass[] = WIFI_PASSWORD_EV;
//const char ssid[] = WIFI_SSID_IS;
//const char pass[] = WIFI_PASSWORD_IS;
//------------------------------------------------------------------

WiFiClient client;

int pinLED = 5;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  Serial.println("Uyandim");
  delay(2000);

  pinMode(pinLED, OUTPUT);

  // Connect or reconnect to WiFi

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected");


  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(pinLED, HIGH);
  }

  delay(3000);

  Serial.println("Uykuya geciyorum");
  delay(2000);


  ESP.deepSleep(15e6);
  delay(20);
}

void loop() {
}
