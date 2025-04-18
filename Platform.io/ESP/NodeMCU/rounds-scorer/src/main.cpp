#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <ctime>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>


#define WIFI_SSID "SONRASI_YOKTU"
#define WIFI_PASSWORD "BuuRA03045025"
#define DATABASE_URL "https://rounds-scorer-default-rtdb.europe-west1.firebasedatabase.app"

const int pinButtonStartMatch = 2;
const int pinButtonScoreForBlue = 5;
const int pinButtonScoreForOrange = 4;

const int pinLEDGreen = 14;
const int pinLEDBlue = 12;
const int pinLEDOrange = 13;

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

DefaultNetwork network;
UserAuth user_auth("AIzaSyB7tLcb93bOOoam8xHshFQSgy0jEGXf3h8", "b@g.com", "123456");
FirebaseApp app;
WiFiClientSecure ssl_auth, ssl_client, ssl_listenOngoingMatchId;
using AsyncClient = AsyncClientClass;
AsyncClient clientAuth(ssl_auth, getNetwork(network)),
    client(ssl_client, getNetwork(network)),
    clientListenOngoingMatchId(ssl_listenOngoingMatchId, getNetwork(network));
RealtimeDatabase Database;

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
String ongoingMatchId;
unsigned long ms = 0;
bool isListeningOngoingMatchStarted = false;

struct Match {
    time_t createdAt{};
    int duration{};
    String id;
    time_t updatedAt{};
    String winner;
};
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void startMatchCB(AsyncResult &aResult) {
  printResult(aResult);

  if (strcmp(aResult.uid().c_str(), "startMatch") == 0) {
    StaticJsonDocument<100> match;
    DeserializationError error = deserializeJson(match, aResult.c_str());

    if (error) {
      Serial.print("JSON deserialization error: ");
      Serial.println(error.c_str());
    }

    String name = match["name"];

    if (name) {
      Serial.print("Name: ");
      Serial.println(name);
      ongoingMatchId = name;
      Serial.print("ongoingMatchId copied: ");
      Serial.println(ongoingMatchId);

      Database.set<String>(client, "/ongoingMatchId", ongoingMatchId);
    } else {
      Serial.println("Name was not found");
    }
  }
}

void getOngoingMatchIdStreamCB(AsyncResult &aResult) {
  printResult(aResult);

  auto &RTDB = aResult.to<RealtimeDatabaseResult>();
  if (RTDB.to<String>().isEmpty()) {
    ongoingMatchId = "";
  } else {
    ongoingMatchId = RTDB.to<String>().c_str();
  }
}

void startMatch(const Match &match) {
  Serial.println("startMatch");

  StaticJsonDocument<140> doc;
  doc["createdAt"] = match.createdAt;
  doc["duration"] = match.duration;
  doc["updatedAt"] = match.updatedAt;
  doc["winner"] = match.winner;

  char jsonString[140];
  serializeJson(doc, jsonString);

  Serial.println(jsonString);

  Database.push<object_t>(client, "/matches", object_t(jsonString), startMatchCB, "startMatch");
}

void voteForTeamCB(AsyncResult &aResult) {
  printResult(aResult);
  auto &RTDB = aResult.to<RealtimeDatabaseResult>();

  Serial.print("voteForTeamCB result: ");
  Serial.println(RTDB.to<String>());
}

void voteForTeam(const String &team) {
  StaticJsonDocument<56> doc;

  if (team == "blue") {
    doc["blue"] = 1;
    doc["orange"] = 0;
  } else {
    doc["blue"] = 0;
    doc["orange"] = 1;
  }

  char jsonString[56];
  serializeJson(doc, jsonString);

  Database.push<object_t>(
      client,
      "/matches/" + ongoingMatchId + "/rounds",
      object_t(jsonString),
      voteForTeamCB,
      "voteForTeam"
  );
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

  Serial.println("Initializing app...");

  ssl_auth.setInsecure();
  ssl_client.setInsecure();
  ssl_client.setInsecure();
  ssl_client.setInsecure();
  ssl_listenOngoingMatchId.setInsecure();
  ssl_auth.setBufferSizes(1024, 1024);
  ssl_client.setBufferSizes(1024, 1024);
  ssl_client.setBufferSizes(1024, 1024);
  ssl_client.setBufferSizes(1024, 1024);
  ssl_listenOngoingMatchId.setBufferSizes(4096, 1024);

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Retrieving time from NTP server...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Time retrieved!");

  initializeApp(clientAuth, app, getAuth(user_auth), asyncCB, "authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  Database.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");
  Database.get(
      clientListenOngoingMatchId, "/ongoingMatchId", getOngoingMatchIdStreamCB, true /* SSE mode (HTTP Streaming) */,
      "ongoingMatchId"
  );
}

void loop() {
  // The async task handler should run inside the main loop
  // without blocking delay or bypassing with millis code blocks.

  app.loop();

  Database.loop();

  if (ongoingMatchId.isEmpty()) {
    digitalWrite(pinLEDGreen, LOW);
  } else {
    digitalWrite(pinLEDGreen, HIGH);
  }

  if (millis() - ms > 10000 && app.ready()) {
    if (digitalRead(pinButtonStartMatch) == LOW && ongoingMatchId.isEmpty()) {
      ms = millis();

      time_t now = time(nullptr);
      Serial.print("Unix time: ");
      Serial.println(now);

      Match newMatch;
      newMatch.createdAt = time(nullptr);
      newMatch.duration = 0;
      newMatch.updatedAt = time(nullptr);
      newMatch.winner = "";

      startMatch(newMatch);
    }

    if (!ongoingMatchId.isEmpty()) {
      if (digitalRead(pinButtonScoreForBlue) == LOW) {
        ms = millis();

        digitalWrite(pinLEDBlue, HIGH);
        voteForTeam("blue");
      } else {
        digitalWrite(pinLEDBlue, LOW);
      }

      if (digitalRead(pinButtonScoreForOrange) == LOW) {
        ms = millis();

        digitalWrite(pinLEDOrange, HIGH);
        voteForTeam("orange");
      } else {
        digitalWrite(pinLEDOrange, LOW);
      }
    }
  }
}

void asyncCB(AsyncResult &aResult) {
  // WARNING!
  // Do not put your codes inside the callback and printResult.

  printResult(aResult);
}

void printResult(AsyncResult &aResult) {
  Serial.print("GENEL ongoingMatchId: ");
  Serial.println(ongoingMatchId);

  if (aResult.isEvent()) {
    Firebase.printf(
        "Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(),
        aResult.appEvent().code());
  }

  if (aResult.isDebug()) {
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
  }

  if (aResult.isError()) {
    Firebase.printf(
        "Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(),
        aResult.error().code());
  }

  if (aResult.available()) {
    auto &RTDB = aResult.to<RealtimeDatabaseResult>();
    if (RTDB.isStream()) {
      Serial.println("----------------------------");
      Firebase.printf("task: %s\n", aResult.uid().c_str());
      Firebase.printf("event: %s\n", RTDB.event().c_str());
      Firebase.printf("path: %s\n", RTDB.dataPath().c_str());
      Firebase.printf("data: %s\n", RTDB.to<const char *>());
      Firebase.printf("type: %d\n", RTDB.type());

      // The stream event from RealtimeDatabaseResult can be converted to the values as following.
      bool v1 = RTDB.to<bool>();
      int v2 = RTDB.to<int>();
      float v3 = RTDB.to<float>();
      double v4 = RTDB.to<double>();
      String v5 = RTDB.to<String>();

    } else {
      Serial.println("----------------------------");
      Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }

    Firebase.printf("Free Heap: %d\n", EspClass::getFreeHeap());
  }
}