#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 8

int pinLED = 2;
int stateButton;

struct Signal {
  byte stateButton;
};

Signal data;

const uint64_t pipe = 0xE8E8F0F0E1LL;
RF24 radio(CE_PIN, CSN_PIN); 

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(pinLED, OUTPUT);
  Serial.println("Nrf24L01 Alıcı Başlatılıyor");

  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening(); 
}

void loop() {
  while (radio.available()) {
    radio.read(&data, sizeof(Signal));

    digitalWrite(pinLED, LOW);
    stateButton = data.stateButton;

    Serial.print("Button state = ");
    Serial.print(stateButton);
    Serial.println();

    if (stateButton == 1) {
      digitalWrite(pinLED, HIGH);
    } else {
      digitalWrite(pinLED, LOW);
    }
  }
}