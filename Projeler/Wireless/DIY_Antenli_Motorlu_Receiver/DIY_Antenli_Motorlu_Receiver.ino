//  3 Channel Receiver | 3 Kanal Alıcı
//  PWM output on pins D3, D5, D6 (Çıkış pinleri)
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

int ch_width_1 = 0;
//*** int ch_width_2 = 0;
//*** int ch_width_3 = 0;

Servo ch1;
//*** Servo ch2;
//*** Servo ch3;

struct Signal {
  //*** byte throttle;      
  byte steering;
  //*** byte aux1;
};

Signal data;
const uint64_t pipeIn = 0xE9E8F0F0E1LL;

// Select CE,CSN pin | CE ve CSN pinlerin seçimi
RF24 radio(9, 10); 

void ResetData() {
  // Define the inicial value of each data input. | Veri girişlerinin başlangıç değerleri
  // The middle position for Potenciometers | Potansiyometreler için orta konum
  data.steering = 127;   // Center | Merkez
  //*** data.throttle = 127;   // Motor Stop | Motor Kapalı
  //*** data.aux1 = 0;         // Center | Merkez
}

void setup() {
  //Set the pins for each PWM signal | Her bir PWM sinyal için pinler belirleniyor.
  ch1.attach(3);
  //*** ch2.attach(5);
  //*** ch3.attach(6);
  //Configure the NRF24 module
  ResetData();
  radio.begin();
  radio.openReadingPipe(1,pipeIn);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening(); //start the radio comunication for receiver | Alıcı olarak sinyal iletişimi başlatılıyor
}

unsigned long lastRecvTime = 0;

void recvData() {
  while (radio.available()) {
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis();   // receive the data | data alınıyor
  }
}

void loop() {
  recvData();

  unsigned long now = millis();

  if (now - lastRecvTime > 1000) {
    ResetData(); // Signal lost.. Reset data | Sinyal kayıpsa data resetleniyor
  }

  ch_width_1 = map(data.steering, 0, 255, 1000, 2000);     // pin D3 (PWM signal)
  //*** ch_width_2 = map(data.throttle, 0, 255, 800, 2200);     // pin D5 (PWM signal)
  //*** ch_width_3 = map(data.aux1, 0, 1, 1000, 2000);           // pin D6 (PWM signal)
  
  // Write the PWM signal | PWM sinyaller çıkışlara gönderiliyor
  ch1.writeMicroseconds(ch_width_1);
  //*** ch2.writeMicroseconds(ch_width_2);
  //*** ch3.writeMicroseconds(ch_width_3);
}