#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "credentials.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET LED_BUILTIN  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//----------------  Fill in your credentails   ---------------------
const char ssid[] = WIFI_SSID_EV;
const char pass[] = WIFI_PASSWORD_EV;
//const char ssid[] = WIFI_SSID_IS;
//const char pass[] = WIFI_PASSWORD_IS;
//------------------------------------------------------------------

const char *host = "api.thingspeak.com";
const char *url = "/apps/thinghttp/send_request?api_key=ID01RLTWTE0EF82T";

int pinButton1 = 0;
int pinButton2 = 2;
int pinButton3 = 14;
int pinButton4 = 12;
int pinLED = 13;

void setup() {
  Serial.begin(115200);
  delay(10);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinButton1, INPUT_PULLUP);
  pinMode(pinButton2, INPUT_PULLUP);
  pinMode(pinButton3, INPUT_PULLUP);
  pinMode(pinButton4, INPUT_PULLUP);
  pinMode(pinLED, OUTPUT);


  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(WIFI_SSID_EV, WIFI_PASSWORD_EV);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (digitalRead(pinButton1) == LOW) {
    digitalWrite(pinLED, HIGH);
    Serial.println('1');
  } else {
    digitalWrite(pinLED, LOW);
  }

  if (digitalRead(pinButton2) == LOW) {
    digitalWrite(pinLED, HIGH);
    Serial.println('2');
  } else {
    digitalWrite(pinLED, LOW);
  }

  if (digitalRead(pinButton3) == LOW) {
    digitalWrite(pinLED, HIGH);
    Serial.println('3');
  } else {
    digitalWrite(pinLED, LOW);
  }

  if (digitalRead(pinButton4) == LOW) {
    digitalWrite(pinLED, HIGH);
    Serial.println('4');
  } else {
    digitalWrite(pinLED, LOW);
  }

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient Client;
  const int httpPort = 80;
  if (!Client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }

  display.clearDisplay();
  display.setTextSize(2);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.cp437(true);          // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 20);
  display.print(WiFi.localIP());

  display.display();
}
