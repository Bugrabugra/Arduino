//ESP TX >> Mega RX(0)
//ESP RX >> Mega TX(0)

#include <ArduinoJson.h>
#include "credentials.h"

// const char ssid[] = WIFI_SSID_EV;
// const char pass[] = WIFI_PASSWORD_EV;
const char ssid[] = WIFI_SSID_IS;
const char pass[] = WIFI_PASSWORD_IS;

char host[16] = "184.106.153.149";
int sayi = 0;

void setup()
{
  Serial.begin(115200); //Seriport'u açıyoruz. Güncellediğimiz

  Serial.println("AT");
  delay(5000);

  if (Serial.find("OK"))
  {
    //connect to your wifi netowork
    startWifi();
  } 
}

void loop() 
{
  httpRequest();
  delay(10000);
}

void httpRequest() 
{
  char start_sentence[44] = "AT+CIPSTART=\"TCP\",\"";
  strcat(start_sentence, host);
  strcat(start_sentence, "\",80");

  Serial.println(start_sentence);  //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz.
  delay(2000);

  if (Serial.find("Error"))
  {
    return false;
  }

  sayi++;
  char postData[116] = "GET /channels/871881/feeds.json?results=1 HTTP/1.1\r\n";
  strcat(postData, "Host: api.thingspeak.com\r\n");
  strcat(postData, "Connection: keep-alive\r\n");
  strcat(postData, "\r\n");

  //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor

  //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
  Serial.print("AT+CIPSEND="); 
  Serial.println(sizeof(postData));

  //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
  if (Serial.find(">")) 
  {
    // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
    Serial.print(postData);
    // delay(2000); //Delayi önce koyunca veriyi çekemedi
    gelenVeri();
    delay(3000);
    Serial.println("AT+CIPCLOSE");

    // çalışma sıklığı saniye olarak
    //delay(device.interval * 1000);
    delay(2500);
  }
}


void startWifi() 
{
  //esp modülü ile bağlantıyı kurabilmişsek modül "AT" komutuna "OK" komutu ile geri dönüş yapıyor.
  Serial.println("AT+CWMODE=3"); //esp modülümüzün WiFi modunu STA şekline getiriyoruz. Bu mod ile modülümüz başka ağlara bağlanabilecek.
  delay(2000);

  char baglantiKomutu[40] = "AT+CWJAP=\"";
  strcat(baglantiKomutu, ssid);
  strcat(baglantiKomutu, "\",\"");
  strcat(baglantiKomutu, pass);
  strcat(baglantiKomutu, "\"");

  Serial.println(baglantiKomutu);
  delay(5000);

  if (Serial.find("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}


void gelenVeri() 
{
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 240;
  DynamicJsonDocument doc(capacity);

  String gelen = "";
  String jsonData = "";



  if (Serial.available() > 0) 
  {
    gelen = Serial.readString();
  }
  delay(1000);

  Serial.println("---------GELEN Veri: -----------");
  Serial.println(gelen);
  Serial.println();

  jsonData = parseJsonStr(gelen);
  Serial.println("---------JSON Parsed-----------");
  Serial.println(jsonData);
  Serial.println();

  DeserializationError error = deserializeJson(doc, jsonData);

  // Test if parsing succeeds.
  if (error) 
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // device init
  JsonObject feeds_0 = doc["feeds"][0];

  const char* feeds_0_created_at = feeds_0["created_at"]; // "2019-09-30T10:52:56Z"
  int feeds_0_entry_id = feeds_0["entry_id"]; // 112
  const char* feeds_0_field1 = feeds_0["field1"]; // "1"

  Serial.println("---------JSON Response-----------");

  Serial.print("created_at: ");
  Serial.println(feeds_0_created_at);
  Serial.print("entry_id: ");
  Serial.println(feeds_0_entry_id);
  Serial.print("field1: ");
  Serial.println(feeds_0_field1);
}

String parseJsonStr(String gelen) 
{
  int find1 = 0, find2 = 0;
  String findChar1 = "{", findChar2 = "}";
  String JsonStr = "";

  find1 = gelen.indexOf(findChar1);
  find2 = gelen.lastIndexOf(findChar2);

  JsonStr = gelen.substring(find1, find2 + 1);
  return JsonStr;
}