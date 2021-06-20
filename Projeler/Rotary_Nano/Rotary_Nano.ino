// -----
// SimplePollRotator.ino - Example for the RotaryEncoder library.
// This class is implemented for use with the Arduino environment.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD 3-Clause License. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 04.02.2021 conditions and settings added for ESP8266
// -----

// This example checks the state of the rotary encoder in the loop() function.
// The current position and direction is printed on output when changed.

// Hardware setup:
// Attach a rotary encoder with output pins to
// * A2 and A3 on Arduino UNO.
// * D5 and D6 on ESP8266 board (e.g. NodeMCU).
// Swap the pins when direction is detected wrong.
// The common contact should be attached to ground.

#include <Arduino.h>
#include <RotaryEncoder.h>

//#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input signals on pin 2 and 3
//#define PIN_IN1 6
//#define PIN_IN2 7

//#elif defined(ESP8266)
// Example for ESP8266 NodeMCU with input signals on pin D5 and D6
//#define PIN_IN1 D5
//#define PIN_IN2 D6

//#endif

#define PIN_IN1 6
#define PIN_IN2 7
#define pinButton 12
#define debounce 300

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

void setup()
{
  pinMode(pinButton, INPUT_PULLUP);
  Serial.begin(115200);
  while (! Serial);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");
} // setup()


// Read the current position of the encoder and print out when changed.
void loop()
{
  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    if (newPos % 2 == 0) {
      Serial.print("pos:");
      Serial.print(newPos/2);
      Serial.print(" dir:");
      Serial.println((int)(encoder.getDirection()));
    }

    pos = newPos;
  } // if

  if (digitalRead(pinButton) == 0) {
    Serial.println("button");
    delay(debounce);
  }
  
} // loop ()

// The End
