#include <SPI.h>                      //the communication interface with the modem
#include "RF24.h"                     //the library which helps us to control the radio modem

int msg[1];
int pinButton = 4;

RF24 radio(7, 8);                    //5 and 10 are a digital pin numbers to which signals CE and CSN are connected.

byte addresses[][6] = {"1Node", "2Node"};

void setup(void)
{
  Serial.begin(9600);

  pinMode(pinButton, INPUT_PULLUP);

  radio.begin();                      //it activates the modem.

  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
}

void loop(void)
{
  msg[0] =  digitalRead(pinButton);
  radio.write(msg, 1);
  Serial.println(msg[0]);
}
