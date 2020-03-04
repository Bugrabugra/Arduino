#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

const char ssid[] = WIFI_SSID_EV; //Your Network SSID
const char pass[] = WIFI_PASSWORD_EV; //Your Network Password

int val;
int pinHallEffect = 3;

WiFiClient client;

unsigned long myChannelNumber = 1009245; //Your Channel Number (Without Brackets)
const char *myWriteAPIKey = THINGSPEAK_1009245_W; //Your Write API Key

void setup()
{
	Serial.begin(9600);
  delay(10);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
}


void loop()
{
	val = digitalRead(pinHallEffect);
	Serial.print(val);
	delay(1000);
	
	ThingSpeak.writeField(myChannelNumber, 1,val, myWriteAPIKey); //Update in ThingSpeak
	delay(5000);
}