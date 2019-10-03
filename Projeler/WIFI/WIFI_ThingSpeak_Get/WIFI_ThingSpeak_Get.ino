//ESP TX >> Mega RX(0)
//ESP RX >> Mega TX(0)

#include <ArduinoJson.h>

#define ag_ismi "KAT3"
#define ag_sifresi "UnV-2019!Wf++"
// #define ag_ismi "SONRASI_YOKTU"
// #define ag_sifresi "BuuRA03045025"


#define host "184.106.153.149"
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
  String start_com = "AT+CIPSTART=\"TCP\",\"";
  start_com += host;
  start_com += "\",80";

  Serial.println(start_com);  //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz.
  delay(2000);

  if (Serial.find("Error"))
  {
    return false;
  }

  sayi++;
  String postData = "GET /channels/871881/feeds.json?results=1 HTTP/1.1\r\n";
  postData += "Host: api.thingspeak.com\r\n";
  postData += "Connection: keep-alive\r\n";
  postData += "\r\n";
  //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor

  //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
  Serial.print("AT+CIPSEND="); 
  Serial.println(postData.length());

  //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
  if (Serial.find(">")) {
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
  String baglantiKomutu = "AT+CWJAP=\"";
  baglantiKomutu += + ag_ismi;
  baglantiKomutu += "\",\"";
  baglantiKomutu += ag_sifresi;
  baglantiKomutu += "\"";

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
  String gelen = "", jsonData = "";

  if (Serial.available() > 0) 
  {
    gelen = Serial.readString();
  }

  Serial.println("---------GELEN Veri: " + String(sayi) + "-----------");
  Serial.println(String(gelen));
  Serial.println();

  jsonData = parseJsonStr(gelen);
  Serial.println("---------JSON Parsed-----------");
  Serial.println(jsonData);
  Serial.println();


  StaticJsonDocument<1000> doc;
  //deserializeJson(doc , jsonData);

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
