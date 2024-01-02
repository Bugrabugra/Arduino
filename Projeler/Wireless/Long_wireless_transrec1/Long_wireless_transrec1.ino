#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

// This is just the way the RF24 library works:
// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8

RF24 radio(7, 8);

byte addresses[][6] = { "1Node", "2Node" };

int pinButton = 3;
int pinLED = 4;

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("THIS IS THE RECEIVER CODE - YOU NEED THE OTHER ARDUINO TO TRANSMIT");

  pinMode(pinLED, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  // Initiate the radio object
  radio.begin();

  // radio.setPALevel(RF24_PA_MIN);
  // RF24_PA_MIN=-18dBm,
  // RF24_PA_LOW=-12dBm,
  // RF24_PA_MED=-6dBM,
  // RF24_PA_HIGH=0dBm

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  // Start the radio listening for data
  radio.startListening();
}

// -----------------------------------------------------------------------------
// We are LISTENING on this device only (although we do transmit a response)
// -----------------------------------------------------------------------------
void loop() {
  // This is what we receive from the other device (the transmitter)
  unsigned char incoming;

  // Is there any data for us to get?
  if (radio.available()) {
    // Go and read the data and put it into that variable
    while (radio.available()) {
      radio.read(&incoming, sizeof(char));
    }
    // No more data to get so send it back but add 1 first just for kicks
    // First, stop listening so we can talk
    radio.stopListening();
    if (incoming == 0) {
      digitalWrite(pinLED, HIGH);
    } else {
      digitalWrite(pinLED, LOW);
    }

    unsigned char outgoing = digitalRead(pinButton);
    radio.write(&outgoing, sizeof(char));

    // Now, resume listening so we catch the next packets.
    radio.startListening();

    // Tell the user what we sent back (the random numer + 1)
    Serial.print("Sent response: ");
    Serial.println(outgoing);
  }
}
