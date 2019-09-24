#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

// This is just the way the RF24 library works:
// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8
RF24 radio(7, 8);

byte addresses[][6] = {"1Node", "2Node"};

int pinButton = 3;
int pinLED = 4;

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  Serial.println("THIS IS THE TRANSMITTER CODE - YOU NEED THE OTHER ARDIUNO TO SEND BACK A RESPONSE");

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
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);

  // Random number seeding (we're going to be sending a single random number)
}

// -----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -----------------------------------------------------------------------------
void loop() 
{
  // Generate a single random character to transmit
  unsigned char outgoing = digitalRead(pinButton);

  // Ensure we have stopped listening (even if we're not) or we won't be able to transmit
  radio.stopListening();

  // Did we manage to SUCCESSFULLY transmit that (by getting an acknowledgement back from the other Arduino)?
  // Even we didn't we'll continue with the sketch, you never know, the radio fairies may help us
  if (!radio.write(&outgoing, sizeof(unsigned char))) 
  {
    Serial.println("No acknowledgement of transmission - receiving radio device connected?");
  }

  // Now listen for a response
  radio.startListening();

  // But we won't listen for long, 200 milliseconds is enough
  unsigned long started_waiting_at = millis();

  // Loop here until we get indication that some data is ready for us to read (or we time out)
  while (!radio.available()) 
  {
    // Oh dear, no response received within our timescale
    if (millis() - started_waiting_at > 200 ) 
    {
      Serial.println("No response received - timeout!");
      return;
    }
  }

  // Now read the data that is waiting for us in the nRF24L01's buffer
  unsigned char incoming;
  radio.read(&incoming, sizeof(unsigned char));
  if (incoming == 0)
  {
    digitalWrite(pinLED, HIGH);
  }
  else
  {
    digitalWrite(pinLED, LOW);
  }
  // Show user what we sent and what we got back
  Serial.print("Sent: ");
  Serial.print(outgoing);
  Serial.print(", received: ");
  Serial.println(incoming);

  // Try again 1s later
  delay(1000);
}
