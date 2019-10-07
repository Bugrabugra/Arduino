#include "credentials.h"

// const char ssid[] = WIFI_SSID_EV;
// const char pass[] = WIFI_PASSWORD_EV;
const char ssid[] = WIFI_SSID_IS;
const char pass[] = WIFI_PASSWORD_IS;	

int pinLED = 4;                                                 

void setup()
{
  pinMode(pinLED, OUTPUT);

  Serial.begin(115200);                                         //Seri haberleşmeyi başlatıyoruz.
  Serial.println("AT");                                         //AT komutu ile modül kontrolünü yapıyoruz.
  while (!Serial.find("OK"))
  {                                                             //Modül hazır olana kadar bekliyoruz.
    Serial.println("AT");
  }
  delay(1000);

  Serial.println("AT+CWMODE=1");                                //Modülümüzü client olarak ayarlıyoruz.
  while (!Serial.find("OK"));                                    //Ayarlamanın tamamlanmasını bekliyoruz.
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + pass + "\"");   //Wifi'a bağlanıyoruz.
  delay(6000);
  while (!Serial.find("OK"));                                    //Bağlanana kadar bekliyoruz.
  Serial.print("AT+CIFSR\r\n");                                 //IP adresini ve MAC adresini okuyoruz.
  Serial.print(espOkuma(1000));                                 //IP ve MAC adresini ekrana yazıyoruz.
  serialTemizle(2000);                                          //Seri haberleşmede kullanmadığımız verileri temizliyoruz.
  Serial.print("AT+CIPMUX=1\r\n");                              //Çoklu bağlantı ayarını yapıyoruz.
  serialTemizle(2000);
  Serial.print("AT+CIPSERVER=1,80\r\n");                        //Server oluşturuyoruz ve 80. porttan dinlemeye başlıyoruz.
  serialTemizle(2000);
}

void loop()
{
  if (Serial.available())
  {                                                             //Seri haberleşme verisi geldiğinde içindeki komutlar çalışır.
    if (Serial.find("+IPD,"))
    {                                                           //Bağlantı isteği geldiğinde içindeki komutlar çalışır.
      delay(200);
      int connectionId = Serial.read() - 48;                    //Bağlantı numarasını okuyoruz.

      String komut = espOkuma(1000);                            //Bağlantı komutlarını alıyoruz.    
      if (komut.indexOf("yak") != -1)
      {                                                         //İleri komutu geldiğinde içerisindeki komutlar çalışır.
        digitalWrite(pinLED, HIGH);
      }
      else if (komut.indexOf("sondur") != -1)
      {                                                         //Geri komutu geldiğinde içerisindeki komutlar çalışır.
        digitalWrite(pinLED, LOW);
      }

      String sayfa = "<h1>LED Yakici</h1><br>";
      sayfa += "<br><a href=\"?yak\"><button><h1>LEDi Yak</h1></button></a>";
      sayfa += "<br><br><a href=\"?sondur\"><button><h1>LEDi Sondur</h1></button></a>";
      komut = "AT+CIPSEND=";                                    //Sayfa verisinin uzunluğu komut değişkenine eşitlenir.
      komut += connectionId;
      komut += ",";
      komut += sayfa.length();
      komut += "\r\n";
      Serial.print(komut);                                      //Komut gönderilir.
      delay(1000);
      Serial.print(sayfa);                                      //Sayfa verisi gönderilir.
      delay(2000);
      komut = "AT+CIPCLOSE=";                                   //Bağlantı sonlandırma komutu ayarlanır.    
      komut += connectionId;
      komut += "\r\n";
      Serial.print(komut);                                      //Bağlantı sonlandırma komutu gönderilir.
    }
  }
}

String espOkuma(long int zamanAsimi)
{                                                               //ESP'den gelen dönütleri okur.
  long int baslangic = millis();
  String gelen;
  while (millis() - baslangic < zamanAsimi)
  {
    if (Serial.available()>0)
    {
      char c = Serial.read();
      gelen += c;
    }
  }
  gelen.replace("AT+", "");
  return gelen;
}

void serialTemizle(long int zamanAsimi)
{                                                               //Seri haberleşmede kullanmadığımız byteları temizler.
  long int baslangic = millis();
  while (millis() - baslangic < zamanAsimi)
  {
    if (Serial.available()>0)
    {
      Serial.read();
    }
  }
}
