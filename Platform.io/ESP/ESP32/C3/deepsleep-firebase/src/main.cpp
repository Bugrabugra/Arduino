#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include "ExampleFunctions.h"

#define WIFI_SSID "SONRASI_YOKTU"
#define WIFI_PASSWORD "BuuRA03045025"
#define API_KEY "AIzaSyAIA1sRnhP4ghzX16w7p34DlAskxlzduII"
#define USER_EMAIL "am-i-at-home@gmail.com"
#define USER_PASSWORD "123456"
#define DATABASE_URL "https://am-i-at-home-default-rtdb.europe-west1.firebasedatabase.app"

#define BUTTON_PIN 5

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

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFiClass::status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  struct tm timeInfo{};
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeInfo, "Time set: %Y-%m-%d %H:%M:%S");

  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  set_ssl_client_insecure_and_buffer(ssl_client);

  Serial.println("Initializing app...");
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

void show_status(const bool status) {
  if (status)
    Serial.println("Success");
  else
    Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());

  // Deep sleep
  esp_deep_sleep_start();
}

void set_await(const int buttonState) {
  // Set the specific value (waits until the value was successfully set)
  // Using Database.set<T>
  Serial.println(String("buttonState: ") + buttonState);

  time_t now;
  struct tm timeInfo{};
  time(&now);
  localtime_r(&now, &timeInfo);

  char timeStr[25];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeInfo);

  const String jsonStr = String("{\"isAtHome\":") + (buttonState ? "false" : "true") + R"(,"ts":")" + timeStr + "\"}";

  Serial.println("JSON -> " + jsonStr);

  // Set json
  Serial.println("Setting the JSON value... ");
  const bool status = Database.set<object_t>(aClient, "/status", object_t(jsonStr.c_str()));
  show_status(status);
}

void loop() {
  // To maintain the authentication and async tasks
  app.loop();

  if (app.ready() && !taskComplete) {
    taskComplete = true;
    Serial.println("------------------------------");
    Serial.println("🕒 Await set values");
    Serial.println("------------------------------");
    set_await(state);
  }
}
