#include <ArduinoJson.h>

#define ag_ismi "KAT3"
#define ag_sifresi "UnV-2019!Wf++"

const String host = "184.106.153.149";
int sayi = 0;

void setup()
{
  Serial.begin(115200); //Seriport'u açıyoruz. Güncellediğimiz
  startWifi();
}

void loop() 
{
  httpRequest();
  delay(5000);
}

void httpRequest() 
{
  Serial.println(String("AT+CIPSTART=\"TCP\",\"") + host + "\",80");  //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz.
  delay(2000);
  sayi++;
  String postData = "GET /channels/871881/feeds.json?results=1 HTTP/1.1\n";
  postData += "Host: " + host + "\n";
  postData += "Connection: keep-alive\n";
  //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor

  //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
  Serial.print("AT+CIPSEND="); Serial.println(postData.length());
  delay(2000);

  //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
  if (Serial.find(">")) {
        // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
        Serial.print(postData);
        delay(2000);
        gelenVeri();
        Serial.println("AT+CIPCLOSE");

        // çalışma sıklığı saniye olarak
    //delay(device.interval * 1000);
        delay(2500);

  }
}
void startWifi() 
{
  Serial.println("AT");
  delay(3000); //ESP ile iletişim için 3 saniye bekliyoruz.

  if (Serial.find("OK")) 
  {
    //esp modülü ile bağlantıyı kurabilmişsek modül "AT" komutuna "OK" komutu ile geri dönüş yapıyor.
    Serial.println("AT+CWMODE=3"); //esp modülümüzün WiFi modunu STA şekline getiriyoruz. Bu mod ile modülümüz başka ağlara bağlanabilecek.
    delay(2000);
    String baglantiKomutu = String("AT+CWJAP=\"") + ag_ismi + "\",\"" + ag_sifresi + "\"";
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
