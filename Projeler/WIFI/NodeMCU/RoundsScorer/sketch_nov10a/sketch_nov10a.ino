#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "SONRASI_YOKTU";
const char* password = "BuuRA03045025";
String api_key = "AIzaSyB7tLcb93bOOoam8xHshFQSgy0jEGXf3h8";
String firestore_project_id = "rounds-scorer";
String firestore_database_url = "https://firestore.googleapis.com/v1/projects/" + firestore_project_id + "/databases/(default)/documents/matches";
HTTPClient http;


void setup() {
// write your initialization code here
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting WiFi...");
  }
  Serial.println("Connected!");

  http.begin(firestore_database_url);
  http.addHeader("Authorization", "Bearer " + api_key);

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String payload = http.getString();
    Serial.println("Firestore yaniti:");
    Serial.println(payload);
  } else {
    Serial.print("HTTP isteği başarısız, kod: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop() {
// write your code here
}