#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>

//ST77XX_BLACK
//ST77XX_WHITE
//ST77XX_RED
//ST77XX_GREEN
//ST77XX_BLUE
//ST77XX_CYAN
//ST77XX_MAGENTA
//ST77XX_YELLOW
//ST77XX_ORANGE

// ST7789 TFT module connections
#define TFT_DC    D1     // TFT DC  pin is connected to NodeMCU pin D1 (GPIO5)
#define TFT_RST   D2     // TFT RST pin is connected to NodeMCU pin D2 (GPIO4)
#define TFT_CS    D8     // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)
// initialize ST7789 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN)(SDA) ---> NodeMCU pin D7 (GPIO13)

const char* ssid = "SONRASI_YOKTU";
const char* password = "BuuRA03045025";
const char* _getLink = "http://jsonplaceholder.typicode.com/todos/1";

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  delay(1000);

  // if the display has CS pin try with SPI_MODE0
  tft.init(240, 240, SPI_MODE2);    // Init ST7789 display 240x240 pixel

  // if the screen is flipped, remove this command
  tft.setRotation(2);

  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);

  Serial.println("Connecting to ");
  Serial.println(ssid);
  tft.println("Connecting to ");
  tft.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.setTextColor(ST77XX_GREEN);
    tft.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  tft.println("");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.println("WiFi connected");

  tft.setTextColor(ST77XX_YELLOW);
  tft.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(_getLink);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
      JSONVar myObject = JSON.parse(payload);
      
      JSONVar keys = myObject.keys();
      tft.setTextColor(ST77XX_BLUE);
      tft.println(myObject[keys[2]]);
    }

    http.end();
  }

  delay(8000);
}
