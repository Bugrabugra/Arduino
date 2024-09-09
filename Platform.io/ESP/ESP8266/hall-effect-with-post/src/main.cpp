// #define ESP_DRD_USE_SPIFFS true

#include <Arduino.h>
//#include <FS.h>
//#include <SPIFFS.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
//#include <ArduinoJson.h>

// JSON configuration file
//#define JSON_CONFIG_FILE "/test/config.json"

//const char *ssid = "SONRASI_YOKTU";          //Enter SSID
//const char *password = "BuuRA03045025";  //Enter Password

const int pinWifi = 0;
const int pinHallEffect = 2;

String windowState;
char endpoint[40];
int state;

WiFiManager wm;

// Flag for saving data
//bool shouldSaveConfig = false;

// Save Config in JSON format
//void saveConfigFile() {
//  Serial.println(F("Saving configuration..."));
//
//  // Create a JSON document
//  StaticJsonDocument<512> json;
//  json["endpoint"] = endpoint;
//
//  // Open config file
//  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
//
//  if (!configFile) {
//    // Error, file did not open
//    Serial.println("failed to open config file for writing");
//  }
//
//  // Serialize JSON data to write to file
//  serializeJsonPretty(json, Serial);
//
//  if (serializeJson(json, configFile) == 0) {
//    // Error writing file
//    Serial.println(F("Failed to write to file"));
//  }
//  // Close file
//  configFile.close();
//}

//bool loadConfigFile() {
//  // Load existing configuration file
//
//  // Uncomment if we need to format filesystem
//  // SPIFFS.format();
//
//  // Read configuration from FS json
//  Serial.println("Mounting File System...");
//
//  // May need to make it begin(true) first time you are using SPIFFS
//  if (SPIFFS.begin(false) || SPIFFS.begin(true)) {
//    Serial.println("mounted file system");
//    if (SPIFFS.exists(JSON_CONFIG_FILE)) {
//      // The file exists, reading and loading
//      Serial.println("reading config file");
//
//      File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
//
//      if (configFile) {
//        Serial.println("Opened configuration file");
//        StaticJsonDocument<512> json;
//
//        DeserializationError error = deserializeJson(json, configFile);
//        serializeJsonPretty(json, Serial);
//
//        if (!error) {
//          Serial.println("Parsing JSON");
//          strcpy(endpoint, json["endpoint"]);
//          return true;
//        } else {
//          // Error loading JSON data
//          Serial.println("Failed to load json config");
//        }
//      }
//    }
//  } else {
//    // Error mounting file system
//    Serial.println("Failed to mount FS");
//  }
//
//  return false;
//}

// Callback notifying us of the need to save configuration
//void saveConfigCallback() {
//  Serial.println("Should save config");
//  shouldSaveConfig = true;
//}

void setup() {
  pinMode(pinWifi, OUTPUT);
  pinMode(pinHallEffect, INPUT);

  // Change to true when testing to force configuration every time we run
//  bool forceConfig = false;

//  bool spiffsSetup = loadConfigFile();

//  if (!spiffsSetup) {
//    Serial.println(F("Forcing config mode as there is no saved config"));
//    forceConfig = true;
//  }

  // Explicitly set Wi-Fi mode
  WiFi.mode(WIFI_STA);

  // Setup Serial monitor
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
//  Wi-Fi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print("*");
//  }

  // Reset settings (only for development)
  wm.resetSettings();

//  wm.setSaveConfigCallback(saveConfigCallback);

  // define a text box, 50 characters maximum
  WiFiManagerParameter ip_text_box("ip_text", "Enter endpoint", "", 50);

  // add custom parameter
  wm.addParameter(&ip_text_box);

  bool res;
  res = wm.autoConnect("AutoConnectAP", "password");

  if (res) {
    if (MDNS.begin("esp8266")) {
      digitalWrite(pinWifi, HIGH);
    }
  }

  // If we get here, we are connected to the Wi-Fi
//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());

  state = digitalRead(pinHallEffect);

  // Let's deal with the user config values

  // Copy the string value
  strcpy(endpoint, ip_text_box.getValue());
//  Serial.print("endpoint: ");
//  Serial.println(endpoint);

  // Save the custom parameters to FS
//  if (shouldSaveConfig) {
//    saveConfigFile();
//  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(pinWifi, HIGH);

    WiFiClient client;
    HTTPClient http;

    http.begin(client, endpoint);  // HTTP
    http.addHeader("Content-Type", "application/json");

    if (digitalRead(pinHallEffect) != state) {
      if (digitalRead(pinHallEffect) == LOW) {
        Serial.println("closed");
        windowState = "closed";
        state = 0;
      } else {
        Serial.println("open");
        windowState = "open";
        state = 1;
      }

      http.POST("{\"window\":\"salon1\",\"state\":\"" + windowState + "\"}");
    }

    http.end();
  }

  delay(3000);
}