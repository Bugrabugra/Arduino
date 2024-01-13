/**********************************************************************************
 *  TITLE: Telegram PIR Motion Camera with DHT11 using ESP32-CAM
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/dP5xG7NcoDE
 *  Related Blog : https://iotcircuithub.com/esp32-projects/
 *  
 *  This code is provided free for project purpose and fair use only.
 *  Please do mail us to techstudycell@gmail.com if you want to use it commercially.
 *  Copyrighted © by Tech StudyCell
 *  
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
 *  
 *  Download Board ESP32 (2.0.5) : https://github.com/espressif/arduino-esp32
 *
 *  Download the libraries 
 *  UniversalTelegramBot Library (1.3.0):  https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
 *  ArduinoJson Library (6.20.0): https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
 *  DHT Library (1.4.4): https://github.com/adafruit/DHT-sensor-library
 **********************************************************************************/


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <ESP32Servo.h>

// Replace with your network credentials
const char* ssid = "SONRASI_YOKTU";      //WiFi Name
const char* password = "BuuRA03045025";  //WiFi Password

// Use @myidbot to find out the chat ID of an individual or a group
// You need to click "start" on a bot before it can message you
// Initialize Telegram BOT
String chatId = "6884651532";
String BOTtoken = "6776449501:AAHpHrzT51BEbAcz_ROO3GIBP0NupfZCbUQ";

bool sendPhoto = false;
bool withFlash = false;
bool shouldReleaseTheFood = false;
int servoDefaultAngle = 140;
int servoReleasedAngle = 110;
int servoReleaseDuration = 500;

WiFiClientSecure clientTCP;

UniversalTelegramBot bot(BOTtoken, clientTCP);
Servo myservo;

// Define GPIOs
#define LED 12
#define SERVO 13
#define FLASH_LED 4

//CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22


String r_msg = "";

const unsigned long BOT_MTBS = 1000;  // mean time between scan messages
unsigned long bot_lasttime;           // last time messages' scan has been done

void handleNewMessages(int numNewMessages);
String sendPhotoTelegram();

String lightOn() {
  if (digitalRead(LED)) {
    return "Light is already ON. /lightOff";
  } else {
    digitalWrite(LED, HIGH);
    delay(100);
    return "Light is ON. /lightOff";
  }
}

String lightOff() {
  if (digitalRead(LED)) {
    digitalWrite(LED, LOW);
    delay(100);
    return "Light is OFF. /lightOn";
  } else {
    return "Light is already OFF. /lightOn";
  }
}

String releaseTheFood() {
  return "Releasing the food...";
}

String sendPhotoTelegram() {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  if (withFlash) {
    digitalWrite(FLASH_LED, HIGH);
    delay(200);
  }

  camera_fb_t* fb = NULL;
  fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }

  Serial.println("Connect to " + String(myDomain));

  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");

    Serial.println("Connected to " + String(myDomain));

    String head = "--IotCircuitHub\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chatId + "\r\n--IotCircuitHub\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--IotCircuitHub--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;

    clientTCP.println("POST /bot" + BOTtoken + "/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=IotCircuitHub");
    clientTCP.println();
    clientTCP.print(head);

    uint8_t* fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024) {
      if (n + 1024 < fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      } else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        clientTCP.write(fbBuf, remainder);
      }
    }

    clientTCP.print(tail);

    esp_camera_fb_return(fb);

    int waitTime = 10000;  // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;


    while ((startTimer + waitTime) > millis()) {
      Serial.print(".");
      delay(100);
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        } else if (c != '\r') {
          getAll += String(c);
        }
        if (state == true) {
          getBody += String(c);
        }
        startTimer = millis();
      }
      if (getBody.length() > 0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  } else {
    getBody = "Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  if (withFlash) {
    digitalWrite(FLASH_LED, LOW);
    withFlash = false;
  }
  return getBody;
}

void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != chatId) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String fromName = bot.messages[i].from_name;
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo request");
    }
    if (text == "/photoWithFlash") {
      sendPhoto = true;
      withFlash = true;
      Serial.println("New photo request");
    }
    if (text == "/lightOn") {
      String r_msg = lightOn();
      bot.sendMessage(chatId, r_msg, "");
    }
    if (text == "/lightOff") {
      String r_msg = lightOff();
      bot.sendMessage(chatId, r_msg, "");
    }
    if (text == "/releaseTheFood") {
      shouldReleaseTheFood = true;
      String r_msg = releaseTheFood();
      bot.sendMessage(chatId, r_msg, "");
    }

    if (text == "/start") {
      String welcome = "Welcome to the ESP32-CAM Telegram Motion Camera.\n";
      welcome += "/photo : Takes a new photo\n";
      welcome += "/photoWithFlash : Takes photo with Flash\n";
      welcome += "/lightOn : Turn ON the Light\n";
      welcome += "/lightOff : Turn OFF the Light\n";
      welcome += "/releaseTheFood : Rotate the Servo\n";
      bot.sendMessage(chatId, welcome, "Markdown");
    }
  }
}

void configInitCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;

  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  /*
   * UXGA   = 1600 x 1200 pixels
   * SXGA   = 1280 x 1024 pixels
   * XGA    = 1024 x 768  pixels
   * SVGA   = 800 x 600   pixels
   * VGA    = 640 x 480   pixels
   * CIF    = 352 x 288   pixels
   * QVGA   = 320 x 240   pixels
   * HQVGA  = 240 x 160   pixels
   * QQVGA  = 160 x 120   pixels
   */

  //  // Drop down frame size for higher initial frame rate
  //  sensor_t * s = esp_camera_sensor_get();
  //  s->set_framesize(s, FRAMESIZE_CIF);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

  sensor_t* s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        //flip it back
    s->set_brightness(s, 1);   //up the brightness just a bit
    s->set_saturation(s, -2);  //lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_XGA);  // default FRAMESIZE_CIF
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  delay(1000);

  pinMode(LED, OUTPUT);
  pinMode(FLASH_LED, OUTPUT);
  myservo.attach(SERVO);
  myservo.write(servoDefaultAngle);

  digitalWrite(LED, LOW);
  //digitalWrite(FLASH_LED, LOW);

  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  //  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  //  connecting_process_timed_out = connecting_process_timed_out * 2;
  //  while (WiFi.status() != WL_CONNECTED) {
  //    Serial.print(".");
  //    digitalWrite(FLASH_LED, HIGH);
  //    delay(250);
  //    digitalWrite(FLASH_LED, LOW);
  //    delay(250);
  //    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
  //    if(connecting_process_timed_out == 0) {
  //      delay(1000);
  //      ESP.restart();
  //    }
  //  }
  //  digitalWrite(FLASH_LED, LOW);

  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Start configuring and initializing the camera...");
  configInitCamera();
  Serial.println("Successfully configure and initialize the camera.");

  delay(1000);
}

void loop() {
  if (sendPhoto) {
    Serial.println("Preparing photo");
    sendPhotoTelegram();
    sendPhoto = false;
  }

  if (shouldReleaseTheFood) {
    Serial.println("Releasing the food");
    myservo.write(servoReleasedAngle);
    delay(servoReleaseDuration);
    myservo.write(servoDefaultAngle);
    shouldReleaseTheFood = false;
  }

  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}
