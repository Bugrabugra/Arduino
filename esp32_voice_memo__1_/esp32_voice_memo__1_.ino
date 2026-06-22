/*
  ESP32S3 Ses Memo - İki buton ile çalışır
  
  Donanım:
  - PDM Mikrofon: DATA=42, CLK=41
  - SD Card CS: 21
  - KAYIT butonu: GPIO1 (veya istediğin pin)
  - SYNC butonu:  GPIO0 (Boot butonu)
  
  Kullanım:
  - KAYIT butonuna bas → 20 sn kayıt → SD'ye yazar
  - SYNC butonuna bas → WiFi'ye bağlanır → bekleyen dosyaları yükler → bağlantıyı keser
  
  Doldurulacak yerler:
  - WIFI_SSID / WIFI_PASSWORD
  - HA_IP (Home Assistant IP adresi)
  - HA_TOKEN (HA Long-Lived Access Token)
*/

#include "ESP_I2S.h"
#include "FS.h"
#include "SD.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"  // Arduino IDE'den yükle: ArduinoJson by Benoit Blanchon

// ============================================================
// AYARLAR - BURAYA DOLDUR
// ============================================================
const char* WIFI_SSID     = "WIFI_ADINIZ";
const char* WIFI_PASSWORD = "WIFI_SIFRENIZ";
const char* HA_IP         = "192.168.1.X";   // HA'nın lokal IP'si
const char* HA_TOKEN      = "ey...";          // HA Long-Lived Token
// ============================================================

#define SD_CS_PIN       21
#define RECORD_BTN_PIN   1   // Kayıt butonu - istediğin pine değiştir
#define SYNC_BTN_PIN     0   // Sync butonu - Boot butonu (GPIO0)
#define LED_PIN          2   // Durum LED'i (varsa)
#define RECORD_SECS     20   // Kayıt süresi (saniye)

// Kayıt durumu
bool isRecording = false;

I2SClass i2s;

// HA webhook URL - kayıt gönderme
String ha_webhook_url = String("http://") + HA_IP + ":8123/api/webhook/esp32_voice_memo";

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(RECORD_BTN_PIN, INPUT_PULLUP);
  pinMode(SYNC_BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("=== ESP32S3 Ses Memo ===");

  // SD başlat
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD kart başlatılamadı!");
    blinkError();
    return;
  }
  Serial.println("SD kart hazır.");

  // I2S mikrofon başlat
  i2s.setPinsPdmRx(42, 41);
  if (!i2s.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("I2S başlatılamadı!");
    blinkError();
    return;
  }
  Serial.println("Mikrofon hazır.");
  Serial.println("KAYIT butonu: ses kaydeder");
  Serial.println("SYNC butonu:  WiFi'ye bağlanıp yükler");
}

void loop() {
  // KAYIT butonuna basıldıysa kayıt yap
  if (digitalRead(RECORD_BTN_PIN) == LOW) {
    delay(50); // debounce
    if (digitalRead(RECORD_BTN_PIN) == LOW) {
      while (digitalRead(RECORD_BTN_PIN) == LOW) delay(10); // bırakılmasını bekle
      recordAndSave();
    }
  }

  // SYNC butonuna basıldıysa WiFi'ye bağlan ve yükle
  if (digitalRead(SYNC_BTN_PIN) == LOW) {
    delay(50); // debounce
    if (digitalRead(SYNC_BTN_PIN) == LOW) {
      while (digitalRead(SYNC_BTN_PIN) == LOW) delay(10); // bırakılmasını bekle
      syncNow();
    }
  }

  delay(20);
}

void syncNow() {
  Serial.println("Sync başlıyor...");
  digitalWrite(LED_PIN, HIGH);

  if (tryConnectWifi()) {
    tryUploadAll();
    WiFi.disconnect();
    Serial.println("WiFi bağlantısı kesildi.");
  } else {
    Serial.println("WiFi bağlanamadı, sync iptal.");
    blinkError();
  }

  digitalWrite(LED_PIN, LOW);
}

// ============================================================
// KAYIT
// ============================================================
void recordAndSave() {
  Serial.println("Kayıt başlıyor (" + String(RECORD_SECS) + " sn)...");
  digitalWrite(LED_PIN, HIGH);

  uint8_t* wav_buffer;
  size_t wav_size;

  wav_buffer = i2s.recordWAV(RECORD_SECS, &wav_size);

  String filename = getNextFilename();
  Serial.println("Dosya: " + filename);

  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Dosya açılamadı!");
    digitalWrite(LED_PIN, LOW);
    return;
  }

  file.write(wav_buffer, wav_size);
  file.close();
  free(wav_buffer);

  digitalWrite(LED_PIN, LOW);
  Serial.println("Kayıt tamamlandı: " + filename);
  blinkOK();
}

String getNextFilename() {
  int n = 1;
  while (true) {
    String name = "/memo_";
    if (n < 100) name += "0";
    if (n < 10)  name += "0";
    name += String(n) + ".wav";
    if (!SD.exists(name)) return name;
    n++;
  }
}

// ============================================================
// WiFi BAĞLANTI
// ============================================================
bool tryConnectWifi() {
  Serial.println("WiFi bağlanıyor: " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nBağlandı! IP: " + WiFi.localIP().toString());
    return true;
  } else {
    Serial.println("\nBağlanamadı.");
    WiFi.disconnect();
    return false;
  }
}

// ============================================================
// YÜKLEME
// ============================================================
void tryUploadAll() {
  if (WiFi.status() != WL_CONNECTED) return;

  File root = SD.open("/");
  if (!root) return;

  bool anyUploaded = false;

  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;

    String name = "/" + String(entry.name());

    // Sadece memo_xxx.wav dosyalarını al, uploaded.txt'dekileri atla
    if (name.indexOf("memo_") >= 0 && name.endsWith(".wav")) {
      if (!isAlreadyUploaded(name)) {
        Serial.println("Yükleniyor: " + name);
        bool success = uploadFile(name);
        if (success) {
          markUploaded(name);
          anyUploaded = true;
          Serial.println("✓ Yüklendi: " + name);
          blinkOK();
        } else {
          Serial.println("✗ Yükleme başarısız: " + name);
        }
        delay(1000); // Sunucuya nefes aldır
      }
    }
    entry.close();
  }
  root.close();

  if (!anyUploaded) {
    Serial.println("Yüklenecek yeni dosya yok.");
  }
}

bool uploadFile(String filepath) {
  File file = SD.open(filepath, FILE_READ);
  if (!file) {
    Serial.println("Dosya açılamadı: " + filepath);
    return false;
  }

  size_t filesize = file.size();
  if (filesize == 0) {
    file.close();
    return false;
  }

  uint8_t* buf = (uint8_t*)malloc(filesize);
  if (!buf) {
    Serial.println("Bellek yetersiz!");
    file.close();
    return false;
  }

  file.read(buf, filesize);
  file.close();

  HTTPClient http;
  http.begin(ha_webhook_url);
  http.addHeader("Authorization", "Bearer " + String(HA_TOKEN));
  http.addHeader("Content-Type", "audio/wav");
  http.addHeader("X-Filename", filepath);
  http.setTimeout(30000); // 30 sn timeout

  int httpCode = http.POST(buf, filesize);
  free(buf);
  http.end();

  Serial.println("HTTP yanıt: " + String(httpCode));
  return (httpCode == 200 || httpCode == 204);
}

// ============================================================
// YÜKLEME TAKIP (uploaded.txt)
// ============================================================
bool isAlreadyUploaded(String filename) {
  File log = SD.open("/uploaded.txt", FILE_READ);
  if (!log) return false;

  while (log.available()) {
    String line = log.readStringUntil('\n');
    line.trim();
    if (line == filename) {
      log.close();
      return true;
    }
  }
  log.close();
  return false;
}

void markUploaded(String filename) {
  File log = SD.open("/uploaded.txt", FILE_APPEND);
  if (log) {
    log.println(filename);
    log.close();
  }
}

// ============================================================
// YARDIMCI
// ============================================================
void blinkOK() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void blinkError() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
}
