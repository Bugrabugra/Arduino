#include <Arduino.h>

/*
  ESP32S3 Voice Memo - Two button operation

  Hardware:
  - PDM Microphone: DATA=42, CLK=41
  - SD Card CS: 21
  - RECORD button: GPIO4 (change to any free pin)
  - SYNC button:   GPIO1 (Boot button)

  Usage:
  - Press RECORD button -> records 20s -> saves to SD card
  - Press SYNC button   -> connects to WiFi -> uploads pending files -> disconnects

  Fill in before flashing:
  - WIFI_SSID / WIFI_PASSWORD
  - HA_IP (Home Assistant local IP)
  - HA_TOKEN (HA Long-Lived Access Token)
*/

#include "ESP_I2S.h"
#include "FS.h"
#include "SD.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "WiFiClientSecure.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// ============================================================
// CONFIGURATION - fill in before flashing
// ============================================================
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* GROQ_API_KEY       = "YOUR_GROQ_API_KEY";
const char* NOTION_TOKEN       = "YOUR_NOTION_TOKEN";
const char* NOTION_DATABASE_ID = "YOUR_NOTION_DATABASE_ID";
// ============================================================

#define SD_CS_PIN       21
#define RECORD_BTN_PIN   4   // Record button - GPIO4 (D3)
#define SYNC_BTN_PIN     2   // Sync button - GPIO1 (D0)
// #define LED_PIN         21   // Status LED (if available)
#define RECORD_SECS     10   // Recording duration in seconds

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BTN_UP 3
#define BTN_DOWN 9
#define BTN_BACK 8
#define BTN_ENTER 7

I2SClass i2s;

// Forward declarations
void recordAndSave();
void syncNow();
bool tryConnectWifi();
String transcribeWithGroq(const String& filepath);
bool sendToNotion(String transcript);
String getDate();
bool isAlreadyUploaded(const String &filename);
void markUploaded(const String &filename);
String getNextFilename();
// void blinkOK();
// void blinkError();

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);

  pinMode(RECORD_BTN_PIN, INPUT_PULLUP);
  pinMode(SYNC_BTN_PIN, INPUT_PULLUP);
  // pinMode(LED_PIN, OUTPUT);
  // digitalWrite(LED_PIN, LOW);

  Serial.println("=== ESP32S3 Voice Memoizer ===");

  // Init SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card mount failed!");
    // blinkError();
    return;
  }
  Serial.println("SD card ready.");

  // Init I2S microphone
  i2s.setPinsPdmRx(42, 41);
  if (!i2s.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("I2S init failed!");
    // blinkError();
    return;
  }
  Serial.println("Microphone ready.");
  Serial.println("RECORD button: start recording");
  Serial.println("SYNC button:   upload to Notion via HA");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  // RECORD button pressed
  if (digitalRead(RECORD_BTN_PIN) == LOW) {
    delay(50); // debounce
    if (digitalRead(RECORD_BTN_PIN) == LOW) {
      while (digitalRead(RECORD_BTN_PIN) == LOW) delay(10); // wait for release
      recordAndSave();
    }
  }

  // SYNC button pressed
  if (digitalRead(SYNC_BTN_PIN) == LOW) {
    delay(50); // debounce
    if (digitalRead(SYNC_BTN_PIN) == LOW) {
      while (digitalRead(SYNC_BTN_PIN) == LOW) delay(10); // wait for release
      syncNow();
    }
  }

  delay(20);
}

// ============================================================
// RECORDING
// ============================================================
void recordAndSave() {
  Serial.println("Recording " + String(RECORD_SECS) + "s...");
  // digitalWrite(LED_PIN, HIGH);

  uint8_t* wav_buffer;
  size_t wav_size;
  wav_buffer = i2s.recordWAV(RECORD_SECS, &wav_size);

  String filename = getNextFilename();
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file!");
    // digitalWrite(LED_PIN, LOW);
    free(wav_buffer);
    return;
  }

  file.write(wav_buffer, wav_size);
  file.close();
  free(wav_buffer);

  // digitalWrite(LED_PIN, LOW);
  Serial.println("Saved: " + filename);
  // blinkOK();
}

String getNextFilename() {
  int n = 1;
  while (true) {
    String name = "/memo_";
    if (n < 100) name += "0";
    if (n < 10)  name += "0";
    name += String(n) + ".wav";
    if (!SD.exists(name)) return name;
    n++;
  }
}

// ============================================================
// SYNC
// ============================================================
void syncNow() {
  Serial.println("Sync started...");
  // digitalWrite(LED_PIN, HIGH);

  if (!tryConnectWifi()) {
    Serial.println("WiFi connection failed.");
    // blinkError();
    // digitalWrite(LED_PIN, LOW);
    return;
  }

  File root = SD.open("/");
  if (!root) { WiFi.disconnect(); return; }

  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    String name = "/" + String(entry.name());
    entry.close();

    if (name.indexOf("memo_") >= 0 && name.endsWith(".wav")) {
      if (!isAlreadyUploaded(name)) {
        Serial.println("Processing: " + name);

        String transcript = transcribeWithGroq(name);
        if (transcript.length() > 0) {
          bool ok = sendToNotion(transcript);
          if (ok) {
            markUploaded(name);
            Serial.println("Done: " + name);
            // blinkOK();
          } else {
            Serial.println("Notion write failed: " + name);
          }
        } else {
          Serial.println("Transcription failed: " + name);
        }

        delay(1000);
      }
    }
  }
  root.close();

  WiFi.disconnect();
  Serial.println("Sync complete. WiFi disconnected.");
  // digitalWrite(LED_PIN, LOW);
}

// ============================================================
// GROQ WHISPER TRANSCRIPTION
// ============================================================
String transcribeWithGroq(const String& filepath) {
  File file = SD.open(filepath, FILE_READ);
  if (!file) return "";

  size_t filesize = file.size();
  uint8_t* buf = static_cast<uint8_t *>(malloc(filesize));
  if (!buf) { file.close(); return ""; }
  file.read(buf, filesize);
  file.close();

  // Build multipart/form-data body manually
  String boundary = "----ESP32Boundary7MA4YWxkTrZu0gW";
  String filename  = filepath.substring(filepath.lastIndexOf('/') + 1);

  // Part 1: file field
  String partHeader =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n"
    "Content-Type: audio/wav\r\n\r\n";

  // Part 2: model field
  String modelPart =
    "\r\n--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"model\"\r\n\r\n"
    "whisper-large-v3-turbo"
    "\r\n";

  // Part 3: language field
  String langPart =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"language\"\r\n\r\n"
    "tr"
    "\r\n";

  // Part 4: response_format field
  String formatPart =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"response_format\"\r\n\r\n"
    "json"
    "\r\n--" + boundary + "--\r\n";

  size_t totalSize = partHeader.length() + filesize +
                     modelPart.length() + langPart.length() + formatPart.length();

  uint8_t* body = static_cast<uint8_t *>(malloc(totalSize));
  if (!body) { free(buf); return ""; }

  size_t offset = 0;
  memcpy(body + offset, partHeader.c_str(), partHeader.length()); offset += partHeader.length();
  memcpy(body + offset, buf, filesize);                            offset += filesize;
  memcpy(body + offset, modelPart.c_str(), modelPart.length());   offset += modelPart.length();
  memcpy(body + offset, langPart.c_str(), langPart.length());     offset += langPart.length();
  memcpy(body + offset, formatPart.c_str(), formatPart.length()); offset += formatPart.length();
  free(buf);

  WiFiClientSecure client;
  client.setInsecure(); // skip cert verification (fine for internal use)

  HTTPClient http;
  http.begin(client, "https://api.groq.com/openai/v1/audio/transcriptions");
  http.addHeader("Authorization", "Bearer " + String(GROQ_API_KEY));
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
  http.setTimeout(30000);

  int httpCode = http.POST(body, totalSize);
  free(body);

  String transcript = "";
  if (httpCode == 200) {
    String response = http.getString();
    Serial.println("Groq response: " + response);
    // Parse {"text":"..."} manually - avoids ArduinoJson dependency
    int start = response.indexOf(R"("text":")") + 8;
    int end   = response.indexOf("\",\"", start);
    if (end < 0) end = response.indexOf("\"}", start);
    if (start > 8 && end > start) {
      transcript = response.substring(start, end);
    }
  } else {
    Serial.println("Groq error " + String(httpCode) + ": " + http.getString());
  }

  http.end();
  return transcript;
}

// ============================================================
// NOTION
// ============================================================
bool sendToNotion(String transcript) {
  transcript.replace("\"", "\\\"");

  String payload =
    "{"
      "\"parent\":{\"database_id\":\"" + String(NOTION_DATABASE_ID) + "\"},"
      "\"properties\":{"
        "\"Name\":{\"title\":[{\"text\":{\"content\":\"" + transcript + "\"}}]},"
        "\"Status\":{\"status\":{\"name\":\"To Do\"}}"
      "}"
    "}";

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, "https://api.notion.com/v1/pages");
  http.addHeader("Authorization", "Bearer " + String(NOTION_TOKEN));
  http.addHeader("Notion-Version", "2022-06-28");
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(15000);

  int httpCode = http.POST(payload);
  Serial.println("Notion response: " + String(httpCode));
  if (httpCode != 200) {
    Serial.println(http.getString());
  }
  http.end();

  return (httpCode == 200);
}

// ============================================================
// DATE via NTP
// ============================================================
String getDate() {
  configTime(0, 0, "pool.ntp.org");
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo, 5000)) {
    return "2000-01-01"; // fallback if NTP fails
  }
  char buf[11];
  strftime(buf, sizeof(buf), "%Y-%m-%d", &timeInfo);
  return String(buf);
}

// ============================================================
// WIFI
// ============================================================
bool tryConnectWifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" connected! IP: " + WiFi.localIP().toString());
    return true;
  }
  WiFi.disconnect();
  return false;
}

// ============================================================
// UPLOAD TRACKING
// ============================================================
bool isAlreadyUploaded(const String &filename) {
  File log = SD.open("/uploaded.txt", FILE_READ);
  if (!log) return false;
  while (log.available()) {
    String line = log.readStringUntil('\n');
    line.trim();
    if (line == filename) { log.close(); return true; }
  }
  log.close();
  return false;
}

void markUploaded(const String& filename) {
  File log = SD.open("/uploaded.txt", FILE_APPEND);
  if (log) { log.println(filename); log.close(); }
}

// ============================================================
// HELPERS
// ============================================================
// void blinkOK() {
//   for (int i = 0; i < 3; i++) {
//     digitalWrite(LED_PIN, HIGH); delay(100);
//     digitalWrite(LED_PIN, LOW);  delay(100);
//   }
// }

// void blinkError() {
//   for (int i = 0; i < 10; i++) {
//     digitalWrite(LED_PIN, HIGH); delay(50);
//     digitalWrite(LED_PIN, LOW);  delay(50);
//   }
// }
