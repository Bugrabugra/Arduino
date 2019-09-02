#include <SPI.h>      //the communication interface with the modem
#include "RF24.h"     //the library which helps us to control the radio modem

RF24 radio(7, 8);     /*This object represents a modem connected to the Arduino.
                      Arguments 5 and 10 are a digital pin numbers to which signals
                      CE and CSN are connected.*/

byte addresses[][6] = {"1Node", "2Node"};

int msg[1];
int pinLED = 5;

void setup()
{
  Serial.begin(9600);

  pinMode(pinLED, OUTPUT);

  radio.begin();                    //it activates the modem.

  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  radio.startListening();           //enable receiving data via modem
}

void loop()
{
  if (radio.available())
  { //checks whether any data have arrived at the address of the modem
    while (radio.available())
    {
      radio.read(msg, 1);
      Serial.println(msg[0]);
      if (msg[0] == 0)
      {
        digitalWrite(pinLED, HIGH);
      }
      else
      {
        digitalWrite(pinLED, LOW);
      }
    }
  }
}
