#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define pinCE 9
#define pinCSN 8

int pinButton = 2;

const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(pinCE, pinCSN);

struct Signal {
  byte stateButton;
};

Signal data;

void setup() {
  Serial.begin(9600);
  pinMode(pinButton, INPUT_PULLUP);

  radio.begin();
  radio.openWritingPipe(pipe);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening(); 
}

void loop() {
  if (digitalRead(pinButton) == 0) {
    data.stateButton = 1;
  } else {
    data.stateButton = 0;
  }
  radio.write(&data, sizeof(Signal));
}