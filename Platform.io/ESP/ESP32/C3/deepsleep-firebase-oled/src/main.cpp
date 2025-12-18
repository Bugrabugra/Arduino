#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "ExampleFunctions.h"

// Firebase
#define WIFI_SSID "SONRASI_YOKTU"
#define WIFI_PASSWORD "BuuRA03045025"
#define API_KEY "AIzaSyAIA1sRnhP4ghzX16w7p34DlAskxlzduII"
#define USER_EMAIL "am-i-at-home@gmail.com"
#define USER_PASSWORD "123456"
#define DATABASE_URL "https://am-i-at-home-default-rtdb.europe-west1.firebasedatabase.app"

// Interrupt
#define BUTTON_PIN 5

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

bool taskComplete = false;
int state;

constexpr long gmtOffset_sec = 3 * 3600;
constexpr int daylightOffset_sec = 0;

// OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  Serial.begin(115200);

  // Start OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  pinMode(BUTTON_PIN, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 display.clearDisplay();

  while (WiFiClass::status() != WL_CONNECTED) {
    display.setCursor(10, 10);
    display.print("Connecting WiFi...");
    display.display();
    delay(300);
  }

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  tm timeInfo{};
  if (!getLocalTime(&timeInfo)) {
    return;
  }

  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  set_ssl_client_insecure_and_buffer(ssl_client);

  initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

  app.getApp<RealtimeDatabase>(Database);

  Database.url(DATABASE_URL);

  state = digitalRead(BUTTON_PIN);

  if (state == LOW) {
    esp_deep_sleep_enable_gpio_wakeup(1ULL << BUTTON_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);
  } else {
    esp_deep_sleep_enable_gpio_wakeup(1ULL << BUTTON_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);
  }
}

void show_status(const bool result, const int buttonState, const char *timeStr) {
  if (result) {
    for (int i = 5; i > 0; i--) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(10, 10);
      display.print(buttonState == LOW ? "At home" : "Not at home");
      display.setCursor(10, 25);
      display.print(timeStr);
      display.display();
      delay(1000);
    }
  } else {
    Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
  }

  display.ssd1306_command(SSD1306_DISPLAYOFF);

  // Deep sleep
  esp_deep_sleep_start();
}

void set_await(const int buttonState) {
  time_t now;
  struct tm timeInfo{};
  time(&now);
  localtime_r(&now, &timeInfo);

  char timeStr[25];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeInfo);

  const String jsonStr = String("{\"isAtHome\":") + (buttonState ? "false" : "true") + R"(,"ts":")" + timeStr + "\"}";

  // Set JSON
  const bool result = Database.set<object_t>(aClient, "/status", object_t(jsonStr.c_str()));

  show_status(result, buttonState, timeStr);
}

void loop() {
  // To maintain the authentication and async tasks
  app.loop();

  if (app.ready() && !taskComplete) {
    taskComplete = true;
    // Serial.println("------------------------------");
    // Serial.println("🕒 Await set values");
    // Serial.println("------------------------------");
    set_await(state);
  }
}
