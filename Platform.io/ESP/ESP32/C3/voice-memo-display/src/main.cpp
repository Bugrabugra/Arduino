#include <Arduino.h>
#include "secrets.h"

/*
  Seeed XIAO ESP32C3 + 2.9" ePaper (GDEY029T94, 296x128)
  Voice Memo "To Do" List Display

  Hardware:
  - Seeed XIAO ESP32C3 on the ePaper driver board for the XIAO expansion board
  - 2.9" e-paper panel, 296x128
  - EPD_CS=D1, EPD_DC=D3, EPD_RST=D0, EPD_BUSY=D5
  - UP button:   D2, to GND (INPUT_PULLUP)
  - DOWN button: D4, to GND (INPUT_PULLUP)

  Behavior:
  - Wakes from deep sleep every 3 hours, only between 09:00 and 24:00 local time;
    outside that window it sleeps until the window opens instead of polling
  - Connects to WiFi (same network, no SSID pre-check) and queries the Notion
    database used by voice-memo for pages with Status = "To Do", newest first
  - Refreshes the e-paper only if the list changed since the last wake
  - Goes back to deep sleep

  Fill in before flashing:
  - WIFI_SSID / WIFI_PASSWORD
  - NOTION_TOKEN / NOTION_DATABASE_ID
*/

#include <SPI.h>
#include <GxEPD2_BW.h>
#include "WiFi.h"
#include "HTTPClient.h"
#include "WiFiClientSecure.h"

#define EPD_CS   D1
#define EPD_DC   D3
#define EPD_RST  D0
#define EPD_BUSY D5

#define DISPLAY_WIDTH  296
#define DISPLAY_HEIGHT 128

#define MAX_ITEMS       100 // Notion's database query page_size hard cap
#define TITLE_BUF_LEN   160

#define ACTIVE_HOUR_START 9  // no updates before this hour (local time)
#define ACTIVE_HOUR_END   24 // no updates from this hour onward (24 = midnight)

constexpr uint64_t SLEEP_INTERVAL_US = 3ULL * 60ULL * 60ULL * 1000000ULL; // 3 hours
constexpr long GMT_OFFSET_SEC = 3 * 3600; // Turkey, no DST
constexpr int DAYLIGHT_OFFSET_SEC = 0;

GxEPD2_BW<GxEPD2_290_GDEY029T94, GxEPD2_290_GDEY029T94::HEIGHT> display(
    GxEPD2_290_GDEY029T94(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

RTC_DATA_ATTR bool hasStoredContent = false;
RTC_DATA_ATTR uint32_t lastContentHash = 0;

// Forward declarations
bool tryConnectWifi();

int fetchToDoTitles(char titles[][TITLE_BUF_LEN], int maxTitles);

String turkishToAscii(const String &input);

uint32_t hashTitles(char titles[][TITLE_BUF_LEN], int count);

void renderList(char titles[][TITLE_BUF_LEN], int count);

int printWrapped(const char *text, int index, int numWidth, int x, int y, int lineHeight,
                  int maxCharsPerLine, bool draw = true);

void goToSleep(uint64_t sleepUs = SLEEP_INTERVAL_US);

uint64_t secondsUntilActiveWindow(const struct tm &timeInfo);

// ============================================================
// SETUP (all work happens here; loop() is never reached)
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500); // let power rails settle after a deep-sleep wake before driving the panel

  SPI.begin();
  display.init(115200);
  display.setRotation(3);

  if (!tryConnectWifi()) {
    Serial.println("WiFi not available, keeping last list on screen.");
    goToSleep();
  }

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, "pool.ntp.org", "time.nist.gov");
  struct tm timeInfo;
  bool gotTime = getLocalTime(&timeInfo, 5000);

  if (gotTime && (timeInfo.tm_hour < ACTIVE_HOUR_START || timeInfo.tm_hour >= ACTIVE_HOUR_END)) {
    Serial.println("Outside active window (09:00-24:00), skipping fetch until it opens.");
    WiFi.disconnect(true);
    goToSleep(secondsUntilActiveWindow(timeInfo) * 1000000ULL);
  }

  static char titles[MAX_ITEMS][TITLE_BUF_LEN]; // static: too large for the task stack
  int count = fetchToDoTitles(titles, MAX_ITEMS);

  WiFi.disconnect(true);

  uint32_t newHash = hashTitles(titles, count);

  if (hasStoredContent && newHash == lastContentHash) {
    Serial.println("No changes since last wake, skipping refresh.");
  } else {
    renderList(titles, count);
    lastContentHash = newHash;
    hasStoredContent = true;
  }

  goToSleep();
}

// Seconds remaining until the next ACTIVE_HOUR_START (09:00), assuming
// timeInfo is currently before that hour today.
uint64_t secondsUntilActiveWindow(const struct tm &timeInfo) {
  int64_t secondsSinceMidnight = timeInfo.tm_hour * 3600LL + timeInfo.tm_min * 60LL + timeInfo.tm_sec;
  int64_t targetSeconds = (int64_t) ACTIVE_HOUR_START * 3600LL;
  int64_t diff = targetSeconds - secondsSinceMidnight;
  return diff > 0 ? (uint64_t) diff : SLEEP_INTERVAL_US / 1000000ULL;
}

void loop() {
  // unreachable: setup() always ends in deep sleep
}

// ============================================================
// WIFI
// ============================================================
bool tryConnectWifi() {
  WiFi.mode(WIFI_STA);

  Serial.println("Connecting to: " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for (int i = 0; i < 20; i++) {
    wl_status_t s = WiFi.status();

    if (s == WL_CONNECTED) return true;
    if (s == WL_CONNECT_FAILED || s == WL_NO_SSID_AVAIL) break;

    delay(300);
  }

  WiFi.disconnect();
  return false;
}

// ============================================================
// NOTION
// ============================================================
int fetchToDoTitles(char titles[][TITLE_BUF_LEN], int maxTitles) {
  int count = 0;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, "https://api.notion.com/v1/databases/" + String(NOTION_DATABASE_ID) + "/query");
  http.addHeader("Authorization", "Bearer " + String(NOTION_TOKEN));
  http.addHeader("Notion-Version", "2022-06-28");
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(15000);

  String payload =
      R"({"filter":{"property":"Status","status":{"equals":"To Do"}},"page_size":)" +
      String(maxTitles) + "}";

  int httpCode = http.POST(payload);

  if (httpCode != 200) {
    Serial.println("Notion query error " + String(httpCode) + ": " + http.getString());
    http.end();
    return 0;
  }

  String response = http.getString();
  http.end();

  int searchFrom = 0;

  while (count < maxTitles) {
    int titleKey = response.indexOf("\"title\":[", searchFrom);
    if (titleKey < 0) break;

    int blockStart = titleKey + 9; // right after "title":[
    int blockEnd = response.indexOf(']', blockStart);
    if (blockEnd < 0) break;

    String block = response.substring(blockStart, blockEnd);

    // A title can be made of multiple rich-text runs; concatenate their plain_text.
    String title = "";
    int runFrom = 0;

    while (true) {
      int keyPos = block.indexOf("\"plain_text\":\"", runFrom);
      if (keyPos < 0) break;

      int textStart = keyPos + 14;
      int textEnd = textStart;

      while (textEnd < (int) block.length()) {
        if (block[textEnd] == '"' && block[textEnd - 1] != '\\') break;
        textEnd++;
      }

      title += block.substring(textStart, textEnd);
      runFrom = textEnd + 1;
    }

    title.replace("\\\"", "\"");
    title.replace("\\n", " ");
    title.trim(); // some existing Notion entries have a stray leading space
    title = turkishToAscii(title);

    if (title.length() > 0) {
      title.toCharArray(titles[count], TITLE_BUF_LEN);
      count++;
    }

    searchFrom = blockEnd + 1;
  }

  return count;
}

// ============================================================
// TURKISH -> ASCII (the built-in e-paper font has no Turkish glyphs;
// UTF-8 multi-byte sequences would otherwise render as garbage and
// break the fixed-width alignment used for word wrapping)
// ============================================================
String turkishToAscii(const String &input) {
  String out;
  out.reserve(input.length());

  size_t i = 0;
  while (i < input.length()) {
    uint8_t b0 = (uint8_t) input[i];

    if (b0 < 0x80) {
      out += (char) b0;
      i += 1;
      continue;
    }

    if ((b0 == 0xC3 || b0 == 0xC4 || b0 == 0xC5) && i + 1 < input.length()) {
      uint8_t b1 = (uint8_t) input[i + 1];
      char replacement = 0;

      if (b0 == 0xC3) {
        switch (b1) {
          case 0x87: replacement = 'C'; break; // Ç
          case 0xA7: replacement = 'c'; break; // ç
          case 0x96: replacement = 'O'; break; // Ö
          case 0xB6: replacement = 'o'; break; // ö
          case 0x9C: replacement = 'U'; break; // Ü
          case 0xBC: replacement = 'u'; break; // ü
          default: break;
        }
      } else if (b0 == 0xC4) {
        switch (b1) {
          case 0x9E: replacement = 'G'; break; // Ğ
          case 0x9F: replacement = 'g'; break; // ğ
          case 0xB0: replacement = 'I'; break; // İ
          case 0xB1: replacement = 'i'; break; // ı
          default: break;
        }
      } else if (b0 == 0xC5) {
        switch (b1) {
          case 0x9E: replacement = 'S'; break; // Ş
          case 0x9F: replacement = 's'; break; // ş
          default: break;
        }
      }

      if (replacement) out += replacement;
      i += 2;
      continue;
    }

    i += 1; // unknown multi-byte sequence: drop this byte, keep scanning
  }

  return out;
}

// ============================================================
// CHANGE DETECTION (survives deep sleep via RTC memory)
// ============================================================
uint32_t hashTitles(char titles[][TITLE_BUF_LEN], int count) {
  uint32_t hash = 2166136261u;

  for (int i = 0; i < count; i++) {
    for (const char *p = titles[i]; *p; p++) {
      hash ^= (uint8_t) *p;
      hash *= 16777619u;
    }
    hash ^= 0xFF; // separator between items
  }

  return hash;
}

// ============================================================
// DISPLAY
// ============================================================
void renderList(char titles[][TITLE_BUF_LEN], int count) {
  const int lineHeight = 10;
  const int maxCharsPerLine = 46;
  const int startY = 2;
  const int numWidth = String(count).length(); // keeps "1." vs "100." the same prefix width

  // Dry run (no drawing): find how many items actually fit, reserving one
  // line for a "+N daha" footer in case the whole list doesn't fit.
  int shown = 0;
  int y = startY;

  for (; shown < count; shown++) {
    int nextY = printWrapped(titles[shown], shown + 1, numWidth, 4, y, lineHeight, maxCharsPerLine, false);
    if (nextY > DISPLAY_HEIGHT - lineHeight) break;
    y = nextY + 2; // gap between items
  }

  bool truncated = shown < count;

  display.setFullWindow();
  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(1);

    int drawY = startY;

    if (count == 0) {
      display.setCursor(4, drawY);
      display.print("Liste bos.");
    }

    for (int i = 0; i < shown; i++) {
      drawY = printWrapped(titles[i], i + 1, numWidth, 4, drawY, lineHeight, maxCharsPerLine, true);
      drawY += 2; // gap between items
    }

    if (truncated) {
      display.setCursor(4, drawY);
      display.print("+" + String(count - shown) + " daha");
    }
  } while (display.nextPage());
}

int printWrapped(const char *text, int index, int numWidth, int x, int y, int lineHeight,
                  int maxCharsPerLine, bool draw) {
  String remaining = text;
  bool firstLine = true;

  String number = String(index);
  while ((int) number.length() < numWidth) number = " " + number;

  String indent;
  for (int i = 0; i < numWidth + 2; i++) indent += ' ';

  while (remaining.length() > 0 && y < DISPLAY_HEIGHT) {
    String prefix = firstLine ? (number + ". ") : indent;
    int charsForLine = maxCharsPerLine - prefix.length();

    String line;
    if ((int) remaining.length() <= charsForLine) {
      line = remaining;
      remaining = "";
    } else {
      int breakAt = remaining.lastIndexOf(' ', charsForLine);
      if (breakAt <= 0) breakAt = charsForLine;
      line = remaining.substring(0, breakAt);
      remaining = remaining.substring(breakAt);
      remaining.trim();
    }

    if (draw) {
      display.setCursor(x, y);
      display.print(prefix + line);
    }

    y += lineHeight;
    firstLine = false;
  }

  return y;
}

// ============================================================
// POWER
// ============================================================
void goToSleep(uint64_t sleepUs) {
  // No explicit hibernate(): a full-window refresh() already calls powerOff(),
  // which is enough and avoids a controller deep-sleep/wake quirk on some
  // panel clones that showed up as inverted colors on the next refresh.
  esp_sleep_enable_timer_wakeup(sleepUs);
  esp_deep_sleep_start();
}
