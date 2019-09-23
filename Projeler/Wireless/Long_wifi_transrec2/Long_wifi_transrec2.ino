#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN

const byte address[][6] = {"00001", "00002"};
boolean stateButton1 = 0;
boolean stateButton2 = 0;

int pinButton = 2;
int pinLED = 9;

void setup()
{
  radio.begin();
  radio.openWritingPipe(address[1]);
  radio.openReadingPipe(1, address[0]);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinLED, OUTPUT); 
}

void loop()
{
  //Okuma
  delay(5);
  radio.startListening();
  if (radio.available())
  {
    radio.read(&stateButton1, sizeof(stateButton1));
    if (stateButton1 == LOW)
    {
      digitalWrite(pinLED, HIGH);
    }
    else
    {
      digitalWrite(pinLED, LOW);
    } 
  }
  //Yazma
  delay(5);
  radio.stopListening();
  stateButton2 = digitalRead(pinButton);
  radio.write(&stateButton2, sizeof(stateButton2));
}