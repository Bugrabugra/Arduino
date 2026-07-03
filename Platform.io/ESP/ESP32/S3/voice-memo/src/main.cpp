#include <Arduino.h>
#include "secrets.h"

/*
  ESP32S3 Voice Memo - Two button operation

  Hardware:
  - PDM Microphone: DATA=42, CLK=41
  - SD Card CS: 21
  - RECORD button: GPIO3
  - SYNC button:   GPIO4
  - LED Success:   GPIO2
  - LED Loading:   GPIO1

  Usage:
  - Press RECORD button -> records 8s -> saves WAV to SD card
  - Press SYNC button   -> connects to WiFi -> transcribes with Groq -> saves to Notion -> disconnects

  Fill in before flashing:
  - WIFI_SSID / WIFI_PASSWORD
  - GROQ_API_KEY
  - NOTION_TOKEN / NOTION_DATABASE_ID
*/

#include "ESP_I2S.h"
#include "FS.h"
#include "SD.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "WiFiClientSecure.h"


#define SD_CS_PIN       21
#define PIN_BUTTON_RECORD   3
#define PIN_BUTTON_SYNC     4
#define PIN_LED_SUCCESS         2
#define PIN_LED_LOADING         1
#define RECORD_SECS     8

I2SClass i2s;

// Forward declarations
void recordAndSave();

void syncNow();

bool tryConnectWifi();

String transcribeWithGroq(const String &filepath);

bool sendToNotion(String transcript);

String getDate();

bool isAlreadyUploaded(const String &filename);

void markUploaded(const String &filename);

String getNextFilename();

void blinkOK();

void blinkLoading(int onMs, int offMs);

void loadingOn();

void loadingOff();

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUTTON_RECORD, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SYNC, INPUT_PULLUP);

  pinMode(PIN_LED_SUCCESS, OUTPUT);
  pinMode(PIN_LED_LOADING, OUTPUT);

  digitalWrite(PIN_LED_SUCCESS, LOW);
  digitalWrite(PIN_LED_LOADING, LOW);

  // Init SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card mount failed!");
    return;
  }

  // Init I2S microphone
  i2s.setPinsPdmRx(42, 41);

  if (!i2s.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("I2S init failed!");
  }
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  // RECORD button pressed
  if (digitalRead(PIN_BUTTON_RECORD) == LOW) {
    delay(50); // debounce

    if (digitalRead(PIN_BUTTON_RECORD) == LOW) {
      while (digitalRead(PIN_BUTTON_RECORD) == LOW) delay(10); // wait for release
      recordAndSave();
    }
  }

  // SYNC button pressed
  if (digitalRead(PIN_BUTTON_SYNC) == LOW) {
    delay(50); // debounce

    if (digitalRead(PIN_BUTTON_SYNC) == LOW) {
      while (digitalRead(PIN_BUTTON_SYNC) == LOW) delay(10); // wait for release
      syncNow();
    }
  }

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "CLEAR") {
      SD.remove("/uploaded.txt");
      File root = SD.open("/");

      while (true) {
        File entry = root.openNextFile();

        if (!entry) break;

        String name = "/" + String(entry.name());
        entry.close();

        if (name.indexOf("memo_") >= 0 && name.endsWith(".wav")) {
          SD.remove(name);
        }
      }
      root.close();
      Serial.println("SD cleared.");
    }
  }

  delay(200);
}

// ============================================================
// RECORDING
// ============================================================
void recordAndSave() {
  Serial.println("Recording " + String(RECORD_SECS) + "s...");
  loadingOn();

  uint8_t *wav_buffer;
  size_t wav_size;
  wav_buffer = i2s.recordWAV(RECORD_SECS, &wav_size);

  String filename = getNextFilename();
  File file = SD.open(filename, FILE_WRITE);

  if (!file) {
    Serial.println("Failed to open file!");
    loadingOff();
    free(wav_buffer);
    return;
  }

  file.write(wav_buffer, wav_size);
  file.close();
  free(wav_buffer);

  loadingOff();
  blinkOK();

  Serial.println("Saved: " + filename);
}

String getNextFilename() {
  int n = 1;

  while (true) {
    String name = "/memo_";
    if (n < 100) name += "0";
    if (n < 10) name += "0";
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

  File root = SD.open("/");
  if (!root) {
    Serial.println("Failed to open SD root.");
    return;
  }

  char pending[64][32];
  int pendingCount = 0;

  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    String name = "/" + String(entry.name());
    entry.close();

    if (name.indexOf("memo_") >= 0 && name.endsWith(".wav")) {
      if (!isAlreadyUploaded(name) && pendingCount < 64) {
        name.toCharArray(pending[pendingCount], 32);
        pendingCount++;
      }
    }
  }
  root.close();

  if (pendingCount == 0) {
    Serial.println("Nothing to sync.");
    return;
  }

  if (!tryConnectWifi()) {
    Serial.println("WiFi connection failed.");
    return;
  }

  for (int i = 0; i < pendingCount; i++) {
    String name = pending[i];

    loadingOn();
    String transcript = transcribeWithGroq(name);
    loadingOff();
    transcript.trim(); // Whisper transcripts often have a stray leading space

    if (transcript.length() > 0) {
      delay(400);
      loadingOn();
      bool ok = sendToNotion(transcript);
      loadingOff();
      if (ok) {
        markUploaded(name);
        blinkOK();
      } else {
        Serial.println("Notion write failed: " + name);
      }
    } else {
      Serial.println("Transcription failed: " + name);
    }

    delay(1000);
  }

  WiFi.disconnect();
  Serial.println("Sync complete.");
}

// ============================================================
// GROQ WHISPER TRANSCRIPTION
// ============================================================
String transcribeWithGroq(const String &filepath) {
  File file = SD.open(filepath, FILE_READ);
  if (!file) return "";

  size_t filesize = file.size();
  uint8_t *buf = static_cast<uint8_t *>(malloc(filesize));

  if (!buf) {
    file.close();
    return "";
  }

  file.read(buf, filesize);
  file.close();

  // Build multipart/form-data body manually
  String boundary = "----ESP32Boundary7MA4YWxkTrZu0gW";
  String filename = filepath.substring(filepath.lastIndexOf('/') + 1);

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

  uint8_t *body = static_cast<uint8_t *>(malloc(totalSize));
  if (!body) {
    free(buf);
    return "";
  }

  size_t offset = 0;
  memcpy(body + offset, partHeader.c_str(), partHeader.length());
  offset += partHeader.length();
  memcpy(body + offset, buf, filesize);
  offset += filesize;
  memcpy(body + offset, modelPart.c_str(), modelPart.length());
  offset += modelPart.length();
  memcpy(body + offset, langPart.c_str(), langPart.length());
  offset += langPart.length();
  memcpy(body + offset, formatPart.c_str(), formatPart.length());
  offset += formatPart.length();
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
    // Parse {"text":"..."} manually - avoids ArduinoJson dependency
    int textKey = response.indexOf(R"("text":")");

    if (textKey >= 0) {
      int start = textKey + 8;
      int end = response.indexOf("\"", start);
      if (end > start) {
        transcript = response.substring(start, end);
      }
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
  WiFi.mode(WIFI_STA);

  WiFi.scanNetworks(true); // async, so we can blink while scanning

  int found;
  while ((found = WiFi.scanComplete()) < 0) {
    blinkLoading(300, 300); // slow blink: scanning for SSID
  }

  bool ssidVisible = false;

  for (int i = 0; i < found; i++) {
    if (WiFi.SSID(i) == WIFI_SSID) {
      ssidVisible = true;
      break;
    }
  }

  WiFi.scanDelete();
  loadingOff();

  if (!ssidVisible) {
    Serial.println("SSID not found, skipping.");
    return false;
  }

  Serial.println("Connecting to: " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for (int i = 0; i < 20; i++) {
    wl_status_t s = WiFi.status();
    Serial.println("WiFi status: " + String(s));

    if (s == WL_CONNECTED) {
      loadingOff();
      return true;
    }

    if (s == WL_CONNECT_FAILED || s == WL_NO_SSID_AVAIL) break;

    blinkLoading(80, 80); // fast double-flicker: connecting to wifi
    blinkLoading(80, 80);
  }

  WiFi.disconnect();

  loadingOff();

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

    if (line == filename) {
      log.close();
      return true;
    }
  }

  log.close();

  return false;
}

void markUploaded(const String &filename) {
  File log = SD.open("/uploaded.txt", FILE_APPEND);

  if (log) {
    log.println(filename);
    log.close();
  }
}

// ============================================================
// HELPERS
// ============================================================
void blinkOK() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(PIN_LED_SUCCESS, HIGH);
    delay(200);
    digitalWrite(PIN_LED_SUCCESS, LOW);
    delay(150);
  }
}

void blinkLoading(const int onMs, const int offMs) {
  digitalWrite(PIN_LED_LOADING, HIGH);
  delay(onMs);
  digitalWrite(PIN_LED_LOADING, LOW);
  delay(offMs);
}

void loadingOn() {
  digitalWrite(PIN_LED_LOADING, HIGH);
}

void loadingOff() {
  digitalWrite(PIN_LED_LOADING, LOW);
}
