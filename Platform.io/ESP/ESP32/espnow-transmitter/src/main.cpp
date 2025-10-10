#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

uint8_t receiverAddress[] = {0xB0, 0x81, 0x84, 0x03, 0x76, 0x98};
unsigned long lastSend = 0;
int counter = 0;

// Data structure
typedef struct struct_message {
  int count;
} struct_message;

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, const esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  WiFiClass::mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Sender ready!");
}

void loop() {
  if (millis() - lastSend > 1000) {
    lastSend = millis();
    myData.count = counter++;

    const esp_err_t result = esp_now_send(receiverAddress, reinterpret_cast<uint8_t *>(&myData), sizeof(myData));
    if (result == ESP_OK) {
      Serial.printf("Sent count: %d\n", myData.count);
    } else {
      Serial.println("Send failed!");
    }
  }
}