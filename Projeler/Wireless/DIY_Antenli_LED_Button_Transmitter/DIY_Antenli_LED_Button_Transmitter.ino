// 6 Channel Transmitter | 6 Kanal Verici
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int pinButton = 2;

const uint64_t pipeOut = 0xE9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xE9E8F0F0E1LL | Bu adres alıcı ile aynı olmalı
RF24 radio(9, 10); // select CE,CSN pin | CE ve CSN pinlerin seçimi

struct Signal {
  byte stateButton;
};

Signal data;

void setup() {
  //Start everything up
  pinMode(pinButton, INPUT_PULLUP);

  radio.begin();
  radio.openWritingPipe(pipeOut);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening(); //start the radio comunication for Transmitter | Verici olarak sinyal iletişimi başlatılıyor
}

void loop() {
  if (digitalRead(pinButton) == 0) {
    data.stateButton = 1;
  } else {
    data.stateButton = 0;
  }
  
  radio.write(&data, sizeof(Signal));
}