#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi info
const char *ssid = "SONRASI_YOKTU"; // Your WiFi SSID
const char *password = "BuuRA03045025"; // Your WiFi password
const char *mqtt_server = "192.168.1.163"; // MQTT broker IP address

unsigned long previousMillis = 0;
const long interval = 5000; // 5 seconds

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) {
  // Function that runs when an MQTT message is received
  String message;
  for (int i = 0; i < length; i++) {
    message += (char) payload[i];
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("From Node-RED:");
  display.println(message);
  display.display();
  delay(3000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Hello!");
  display.display();
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 initialization failed!"));
    for (;;);
  }

  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello!");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }

  display.println("\nWiFi connected!");
  display.display();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    Serial.print("Broker address: ");
    Serial.println(mqtt_server);
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Not connected");

    if (client.connect("XIAO_ESP32C3")) {
      Serial.println("Connected to MQTT broker");
      if (client.subscribe("xiao/display")) {
        Serial.println("Subscribed to xiao/display");
      } else {
        Serial.println("Failed to subscribe to topic!");
      }

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("MQTT connected!");
      display.display();
    } else {
      Serial.print("Connection failed, error code: ");
      Serial.println(client.state());
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    String message = "ESP32 active - Running for " + String(millis() / 1000) + " seconds";
    client.publish("esp32/status", message.c_str());
    Serial.println("Message sent: " + message);
  }
}
