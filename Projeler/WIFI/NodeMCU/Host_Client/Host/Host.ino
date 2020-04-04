#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

// Set your access point network credentials
const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int pinButton = 2;
int pinLED = 4;
int stateButton = 1;

String readStateButton()
{
  return String(stateButton);
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(pinButton, INPUT_PULLUP);

  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/LED", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/plain", readStateButton().c_str());
  });

  // Start server
  server.begin();
}


void loop()
{
  if (digitalRead(pinButton) == 0)
  {
    delay(500);
    if (stateButton == 0)
    {
      stateButton = 1;
      Serial.println(stateButton);
    }
    else
    {
      stateButton = 0;
      Serial.println(stateButton);
    } 
  }
}
