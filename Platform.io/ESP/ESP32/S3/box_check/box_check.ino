/*
  Box Check Prototype - Seeed XIAO ESP32-S3 Sense
  -----------------------------------------------
  The ESP32 serves camera frames and the web UI on the local network.
  QR decoding runs in the browser (jsQR); the part list, counters and
  green marking are also handled in the browser.

  Arduino IDE settings:
    Board: XIAO_ESP32S3
    PSRAM: OPI PSRAM   (must be enabled)
  Then open the address printed on the Serial Monitor (115200) in a browser.
*/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "index_html.h"

// ---- Wi-Fi credentials ----
const char* WIFI_SSID = "SONRASI_YOKTU";
const char* WIFI_PASS = "BuuRA03045025";

// ---- XIAO ESP32-S3 Sense camera pins ----
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  10
#define SIOD_GPIO_NUM  40
#define SIOC_GPIO_NUM  39
#define Y9_GPIO_NUM    48
#define Y8_GPIO_NUM    11
#define Y7_GPIO_NUM    12
#define Y6_GPIO_NUM    14
#define Y5_GPIO_NUM    16
#define Y4_GPIO_NUM    18
#define Y3_GPIO_NUM    17
#define Y2_GPIO_NUM    15
#define VSYNC_GPIO_NUM 38
#define HREF_GPIO_NUM  47
#define PCLK_GPIO_NUM  13

WebServer server(80);

bool initCamera() {
  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_SVGA;    // 800x600. Try XGA (1024x768) for small QR codes
  config.jpeg_quality = 10;                // lower = sharper image, larger file
  config.fb_count     = 2;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.grab_mode    = CAMERA_GRAB_LATEST; // always return the freshest frame

  if (esp_camera_init(&config) != ESP_OK) return false;

  sensor_t* sensor = esp_camera_sensor_get();
  sensor->set_sharpness(sensor, 2);  // slightly sharper edges help QR detection
  sensor->set_contrast(sensor, 1);
  return true;
}

void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
}

void handleCapture() {
  camera_fb_t* frame = esp_camera_fb_get();
  if (!frame) { server.send(503, "text/plain", "failed to capture frame"); return; }
  WiFiClient client = server.client();
  client.printf("HTTP/1.1 200 OK\r\n"
                "Content-Type: image/jpeg\r\n"
                "Content-Length: %u\r\n"
                "Cache-Control: no-store\r\n"
                "Connection: close\r\n\r\n", frame->len);
  client.write(frame->buf, frame->len);
  esp_camera_fb_return(frame);
}

// /resolution?size=vga | svga | xga  -> lets the UI change the frame size
void handleResolution() {
  String size = server.arg("size");
  framesize_t frameSize = FRAMESIZE_SVGA;
  if (size == "vga") frameSize = FRAMESIZE_VGA;
  else if (size == "xga") frameSize = FRAMESIZE_XGA;
  sensor_t* sensor = esp_camera_sensor_get();
  sensor->set_framesize(sensor, frameSize);
  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  if (!psramFound()) Serial.println("WARNING: PSRAM not found. Set Tools > PSRAM > OPI PSRAM.");
  if (!initCamera()) { Serial.println("Camera init failed!"); while (true) delay(1000); }

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);  // reduces latency for frame requests
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.println();
  Serial.print("UI: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.on("/resolution", handleResolution);
  server.begin();
}

void loop() {
  server.handleClient();
}
