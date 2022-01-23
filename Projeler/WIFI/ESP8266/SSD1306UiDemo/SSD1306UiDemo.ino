#include <Wire.h> // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h"
#include "OLEDDisplayUi.h"
#include "images.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "credentials.h"


SSD1306 display(0x3c, 0, 2);
OLEDDisplayUi ui(&display);

const char ssid[] = WIFI_SSID_EV;
const char password[] = WIFI_PASSWORD_EV;
const char *host = "https://timeapi.io/api/Time/current/zone?timeZone=Turkey"; //We read the data from this host
int port = 443;

String Data_Raw;
int pin1 = 1;
int pin3 = 3;
bool fetched = false;

WiFiClientSecure client;
HTTPClient http;

void drawProgress(OLEDDisplay *display, int percentage, String label) {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 10, label);
  display->drawProgressBar(2, 28, 124, 10, percentage);
  display->display();
  delay(400);
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) {
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y
  display->drawXbm(x + 34, y + 0, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 33, y + 33, "Connecting to");
  display->drawString(x + 20, y + 42, ssid);

  if (WiFi.status() == WL_CONNECTED) {
    display->clear();
    display->drawXbm(x + 34, y + 0, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    display->drawString(x + 35, y + 33, "Connected!");
    display->drawString(x + 20, y + 42, "IP: " + WiFi.localIP().toString());
  }
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format

  if (digitalRead(pin1) == LOW) {
    ui.nextFrame();
  }

  if (digitalRead(pin3) == LOW) {
    ui.previousFrame();
  }
  // !fetched &&

  if (!fetched && ui.getUiState()->frameState == FIXED) {
    client.setInsecure(); //the magic line, use with caution
    client.connect(host, port);
    // client.setFingerprint(fingerprint);
    http.begin(client, host);
    // https.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        Data_Raw = http.getString();
      }

      StaticJsonDocument<384> doc;
      DeserializationError error = deserializeJson(doc, Data_Raw);
      const char* time = doc["time"]; // "01:50"

      display->clear();
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->setFont(ArialMT_Plain_10);
      display->drawString(x + 0, y + 0, "Result: ");
      display->setFont(ArialMT_Plain_16);
      display->drawString(x + 0, y + 15, time);

      if (digitalRead(pin1) == LOW) {
        ui.nextFrame();
      }

      if (digitalRead(pin3) == LOW) {
        ui.previousFrame();
      }
      http.end();
    }
  }
}

// void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) {
  // // Text alignment demo
  // display->setFont(ArialMT_Plain_10);
  // display->drawString(x + 0, y + 0, "Connecting to");
  // display->drawString(x + 0, y + 10, password);

  // if (http.begin(host, httpPortRead, thinghttp_address)) {
  //   int httpCode = http.GET();
  //   if (httpCode > 0) {
  //     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
  //       Data_Raw = http.getString();

  //       display->clear();
  //       display->setFont(ArialMT_Plain_10);
  //       display->drawString(x + 0, y + 0, Data_Raw);
  //       display->setFont(ArialMT_Plain_16);
  //       display->drawString(x + 0, y + 15, "IP: " + WiFi.localIP().toString());
  //       display->display();
  //       delay(2000);
  //     }
  //   }
  //   http.end();
  // }
// }

// void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//   // Demo for drawStringMaxWidth:
//   // with the third parameter you can define the width after which words will be wrapped.
//   // Currently only spaces and "-" are allowed for wrapping
//   display->setTextAlignment(TEXT_ALIGN_LEFT);
//   display->setFont(ArialMT_Plain_10);
//   display->drawStringMaxWidth(0 + x, 10 + y, 128, "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore.");
// }

// void drawFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

// }

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = {drawFrame1, drawFrame2};

// how many frames are there?
int frameCount = 2;

// Overlays are statically drawn on top of a frame eg. a clock
// OverlayCallback overlays[] = { msOverlay };
// int overlaysCount = 1;

void setup() {
  // Wire.pins(0, 2);// yes, see text
  // Wire.begin(0, 2);// 0=sda, 2=scl

  pinMode(pin1, INPUT_PULLUP);
  pinMode(pin3, INPUT_PULLUP);
  display.init();

  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // Disable auto transition
  ui.disableAutoTransition();

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  // ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();

  WiFi.begin(ssid, password);
}

void loop() {
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
  }

  if (digitalRead(pin1) == LOW) {
    ui.nextFrame();
  }

  if (digitalRead(pin3) == LOW) {
    ui.previousFrame();
  }
}
