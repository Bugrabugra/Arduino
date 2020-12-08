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

void setup()
{
  WidgetLED led1(V1);


// V1 LED Widget is blinking

  // if (led1.getValue()) {
  //   led1.off();
  //   Serial.println("LED on V1: off");
  // } else {
  //   led1.on();
  //   Serial.println("LED on V1: on");
  // }


  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  Serial.println(Blynk.connected());

  if (Blynk.connected())
  {
    led1.on();
  }
  
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
