/**
 * This example is for new users which are familiar with other legacy Firebase libraries.
 *
 * The example shows how to listen the data changes in your Firebase Realtime database
 * while the database was set periodically.
 *
 * All functions used in this example are non-blocking (async) functions.
 *
 * This example will use the database secret for privilege Realtime database access which does not need
 * to change the security rules or it can access Realtime database no matter what the security rules are set.
 *
 * This example is for ESP32, ESP8266 and Raspberry Pi Pico W.
 *
 * You can adapt the WiFi and SSL client library that are available for your devices.
 *
 * For the ethernet and GSM network which are not covered by this example,
 * you have to try another elaborate examples and read the library documentation thoroughly.
 *
 */

#include <Arduino.h>

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)

#include <ESP8266WiFi.h>

#endif

#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <ctime>
#include <ArduinoJson.h>

#define WIFI_SSID "SONRASI_YOKTU"
#define WIFI_PASSWORD "BuuRA03045025"

#define DATABASE_SECRET "AcVnXIVOBniTJK4ShvXcNxYd3KcMZJTtw462EBXw"
#define DATABASE_URL "https://rounds-scorer-default-rtdb.europe-west1.firebasedatabase.app"

const int pinButtonStartMatch = 2;
const int pinButtonScoreForBlue = 5;
const int pinButtonScoreForOrange = 4;

const int pinLEDGreen = 14;
const int pinLEDBlue = 12;
const int pinLEDOrange = 13;

// The SSL client used for secure server connection.
WiFiClientSecure ssl1, ssl2, ssl3;

// The default network config object that used in this library.
DefaultNetwork network;

// The client, aka async client, is the client that handles many tasks required for each operation.
AsyncClientClass clientOngoingMatchId(ssl1, getNetwork(network)),
    clientMatches(ssl2, getNetwork(network)),
    clientOngoingMatch(ssl3, getNetwork(network));

// The authentication task handler, aka FirebaseApp.
FirebaseApp app;

// The Realtime database class object that provides the functions.
RealtimeDatabase Database;

// The class that stores the operating result, aka AsyncResult.
AsyncResult resultOngoingMatchId, result2;

// The legacy token provider class used for authentication initialization.
LegacyToken dbSecret(DATABASE_SECRET);

struct Match {
    time_t createdAt{};
    int duration{};
    String id;
    time_t updatedAt{};
    String winner;
};

unsigned long ms = 0;
String ongoingMatchId;
bool isListeningOngoingMatchStarted = false;

void startMatchCB(AsyncResult &aResult) {
  Serial.print("startMatch uid: ");
  Serial.println(aResult.uid().c_str());

  if (strcmp(aResult.uid().c_str(), "startMatch") == 0) {
    StaticJsonDocument<128> match;

    DeserializationError error = deserializeJson(match, aResult.c_str());

    if (error) {
      Serial.print("JSON deserialization error: ");
      Serial.println(error.c_str());
    }

    const char *name = match["name"];

    if (name) {
      Serial.print("Name: ");
      Serial.println(name);
      ongoingMatchId = name;
      Database.set<String>(clientOngoingMatchId, "/ongoingMatchId", ongoingMatchId);
    } else {
      Serial.println("Name was not found");
    }
  }
}

void startMatch(const Match &match) {
  StaticJsonDocument<256> doc;
  doc["createdAt"] = match.createdAt;
  doc["duration"] = match.duration;
  doc["updatedAt"] = match.updatedAt;
  doc["winner"] = match.winner;

  String jsonString;
  serializeJson(doc, jsonString);

  Database.push<object_t>(clientMatches, "/matches", object_t(jsonString), startMatchCB, "startMatch");
}

void voteForTeamCB(AsyncResult &aResult) {
  auto &RTDB = aResult.to<RealtimeDatabaseResult>();

  Serial.print("voteForTeamCB result: ");
  Serial.println(RTDB.to<String>());
}

void voteForTeam(const String &team) {
  StaticJsonDocument<256> doc;

  if (team == "blue") {
    doc["blue"] = 1;
    doc["orange"] = 0;
  } else {
    doc["blue"] = 0;
    doc["orange"] = 1;
  }

  String jsonString;
  serializeJson(doc, jsonString);

  Database.push<object_t>(
      clientOngoingMatch,
      "/matches/" + ongoingMatchId + "/rounds",
      object_t(jsonString),
      voteForTeamCB,
      "voteForTeam"
  );
}

void getOngoingMatchStreamCB(AsyncResult &aResult) {
  auto &RTDB = aResult.to<RealtimeDatabaseResult>();

  Serial.print("ongoing match stream: ");
  Serial.println(RTDB.to<String>());

  Serial.print("ongoing match not stream: ");
  Serial.println(aResult.c_str());
}

void getOngoingMatchIdStreamCB(AsyncResult &aResult) {
  auto &RTDB = aResult.to<RealtimeDatabaseResult>();
  if (RTDB.to<String>().isEmpty() || RTDB.to<String>() == "null") {
    ongoingMatchId = "";
  } else {
    ongoingMatchId = RTDB.to<String>();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinButtonStartMatch, INPUT_PULLUP);
  pinMode(pinButtonScoreForBlue, INPUT_PULLUP);
  pinMode(pinButtonScoreForOrange, INPUT_PULLUP);

  pinMode(pinLEDGreen, OUTPUT);
  pinMode(pinLEDBlue, OUTPUT);
  pinMode(pinLEDOrange, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  ssl1.setInsecure();
  ssl2.setInsecure();
  ssl3.setInsecure();

#if defined(ESP8266)
  ssl1.setBufferSizes(1024, 1024);
  ssl2.setBufferSizes(1024, 1024);
  ssl3.setBufferSizes(1024, 1024);
#endif

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Retrieving time from NTP server...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Time retrieved!");

  // Initialize the authentication handler.
  initializeApp(clientOngoingMatchId, app, getAuth(dbSecret));

  // Binding the authentication handler with your Database class object.
  app.getApp<RealtimeDatabase>(Database);

  // Set your database URL
  Database.url(DATABASE_URL);

  // Initiate the Stream connection to listen the data changes.
  // This function can be called once.
  // The Stream was connected using async get function (non-blocking) which the result will assign to the function in this case.
  Database.get(
      clientOngoingMatchId, "/ongoingMatchId", getOngoingMatchIdStreamCB,
      true, "getOngoingMatchId" /* this option is for Stream connection */);
}

void loop() {
  // Polling for internal task operation
  // This required for Stream in this case.
  Database.loop();

//  app.loop();
  // We don't have to poll authentication handler task using app.loop() as seen in other examples
  // because the database secret is the privilege access key that never expired.

  // Set the random int value to "/test/stream/int" every 20 seconds.

  Serial.println(ongoingMatchId);

  if (!isListeningOngoingMatchStarted && !ongoingMatchId.isEmpty()) {
    Database.get(
        clientOngoingMatch, "/matches/" + ongoingMatchId, getOngoingMatchStreamCB,
        true, "getOngoingMatch" /* this option is for Stream connection */);
    isListeningOngoingMatchStarted = true;
  }

  if (!ongoingMatchId.isEmpty()) {
    digitalWrite(pinLEDGreen, HIGH);
  } else {
    digitalWrite(pinLEDGreen, LOW);
  }


  if (millis() - ms > 5000 || ms == 0) {
    if (!ongoingMatchId.isEmpty()) {
      if (digitalRead(pinButtonScoreForBlue) == LOW) {
        digitalWrite(pinLEDBlue, HIGH);
        voteForTeam("blue");
      } else {
        digitalWrite(pinLEDBlue, LOW);
      }

      if (digitalRead(pinButtonScoreForOrange) == LOW) {
        digitalWrite(pinLEDOrange, HIGH);
        voteForTeam("orange");
      } else {
        digitalWrite(pinLEDOrange, LOW);
      }

      ms = millis();
    }
  }

  if (millis() - ms > 5000 || ms == 0) {
    if (digitalRead(pinButtonStartMatch) == LOW && ongoingMatchId.isEmpty()) {
      ms = millis();

      time_t now = time(nullptr);
      Serial.print("Unix time: ");
      Serial.println(now);

      // We set the data with this non-blocking set function (async) which the result was assign to the function.
      // Database.set<int>(clientMatches, "/count", now, result2);

      Match newMatch;
      newMatch.createdAt = time(nullptr);
      newMatch.duration = 0;
      newMatch.updatedAt = time(nullptr);
      newMatch.winner = NULL;

      startMatch(newMatch);
    } else {
      digitalWrite(pinLEDGreen, LOW);
    }
  }
}