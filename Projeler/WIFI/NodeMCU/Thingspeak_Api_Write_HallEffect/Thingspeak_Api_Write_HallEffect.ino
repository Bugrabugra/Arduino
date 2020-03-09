#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include "credentials.h"

const char ssid[] = WIFI_SSID_EV; //Your Network SSID
const char pass[] = WIFI_PASSWORD_EV; //Your Network Password

int pinHallEffect = 4;
int pinLED = 13;
int valHallEffect;

WiFiClient client;

unsigned long myChannelNumber = 1009245; //Your Channel Number (Without Brackets)
const char *myWriteAPIKey = THINGSPEAK_1009245_W; //Your Write API Key

void setup()
{
  Serial.begin(9600);
  delay(10);

  pinMode(pinHallEffect, INPUT);
  pinMode(pinLED, OUTPUT);

  // Connect to WiFi network
  WiFi.begin(WIFI_SSID_EV, WIFI_PASSWORD_EV);
  ThingSpeak.begin(client);
}


void loop()
{
  valHallEffect = digitalRead(pinHallEffect);
  Serial.print(valHallEffect);

  if (valHallEffect == 0)
  {
    ThingSpeak.writeField(myChannelNumber, 1, valHallEffect, myWriteAPIKey);
    digitalWrite(pinLED, HIGH);
    delay(17000);
  }
  else
  {
    digitalWrite(pinLED, LOW);
  }
}