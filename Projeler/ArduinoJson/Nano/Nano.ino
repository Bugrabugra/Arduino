#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 240;
  DynamicJsonDocument jsonBuffer(capacity);

  //ÖRNEK VERİ
  const char* json = "{\"channel\":{\"id\":871881,\"name\":\"ESP8266-01 Button\",\"latitude\":\"0.0\",\"longitude\":\"0.0\",\"field1\":\"buttonState\",\"created_at\":\"2019-09-25T11:38:03Z\",\"updated_at\":\"2019-09-25T13:15:39Z\",\"last_entry_id\":121},\"feeds\":[{\"created_at\":\"2019-10-01T18:21:58Z\",\"entry_id\":121,\"field1\":\"1\"}]}";

  DeserializationError error = deserializeJson(jsonBuffer, json);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  JsonObject feeds_0 = jsonBuffer["feeds"][0];

  //Verinin parçalanmış hali
  const char* feeds_0_created_at = feeds_0["created_at"];  // "2019-09-30T10:52:56Z"
  int feeds_0_entry_id = feeds_0["entry_id"];              // 112
  const char* feeds_0_field1 = feeds_0["field1"];          // "1"

  Serial.println("---------JSON Response-----------");

  //Monitörden okuyalım
  Serial.print("created_at: ");
  Serial.println(feeds_0_created_at);
  Serial.print("entry_id: ");
  Serial.println(feeds_0_entry_id);
  Serial.print("field1: ");
  Serial.println(feeds_0_field1);
}

void loop() {
  //programınız
}