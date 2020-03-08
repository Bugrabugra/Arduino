#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include "credentials.h"

const char ssid[] = WIFI_SSID_EV; //Your Network SSID
const char pass[] = WIFI_PASSWORD_EV; //Your Network Password

int pinButton = 2; //*******************
int valButton; //*******************
// int val;
// int pinHallEffect = 3;

WiFiClient client;

unsigned long myChannelNumber = 1009245; //Your Channel Number (Without Brackets)
const char *myWriteAPIKey = THINGSPEAK_1009245_W; //Your Write API Key

void setup()
{
  pinMode(pinButton, INPUT_PULLUP);

  Serial.begin(9600);
  delay(10);

  // Connect to WiFi network
  WiFi.begin(WIFI_SSID_EV, WIFI_PASSWORD_EV);
  ThingSpeak.begin(client);
}


void loop()
{
  valButton = digitalRead(pinButton); //*******************
  // val = digitalRead(pinHallEffect);

  // Serial.print(val);
  Serial.print(valButton);

  delay(1000);
  
  ThingSpeak.writeField(myChannelNumber, 1,valButton, myWriteAPIKey); //*******************
  // ThingSpeak.writeField(myChannelNumber, 1,val, myWriteAPIKey); //Update in ThingSpeak
  delay(5000);
}
