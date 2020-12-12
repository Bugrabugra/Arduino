#include "ESP8266WiFi.h"
#include "credentials.h"

const char* ssid = WIFI_SSID_EV; //Enter SSID
const char* password = WIFI_PASSWORD_EV; //Enter Password
int pinLED = 3;

void setup(void)
{ 
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address
}

void loop() 
{
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(pinLED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);              // wait for a second
    digitalWrite(pinLED, LOW);    // turn the LED off by making the voltage LOW
    delay(1000); 
  }
}
