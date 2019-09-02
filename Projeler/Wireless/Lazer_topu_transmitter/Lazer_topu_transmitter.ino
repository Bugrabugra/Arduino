#include <SPI.h>
#include "RF24.h"

int msg[1];
int pinLaser = 9;
int pinButton = 4;

RF24 radio(7, 8); //CE,CSN

byte addresses[][6] = {"1Node", "2Node"};

void setup(void)
{
  Serial.begin(9600);

  pinMode(pinLaser, INPUT);
  pinMode(pinButton, INPUT_PULLUP);

  radio.begin();

  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
}

void loop(void)
{
  msg[0] =  digitalRead(pinLaser);
  //msg[0] =  digitalRead(pinButton);

  if (msg[0] == 0)
  {
    radio.write(msg, 1);
    delay(2000);
  }

  Serial.println(msg[0]);
}
