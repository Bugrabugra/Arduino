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
  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinLED, OUTPUT);

}
void loop()
{
  //Yazma
  delay(5);
  radio.stopListening();
  stateButton1 = digitalRead(pinButton);
  radio.write(&stateButton1, sizeof(stateButton1));
  
  delay(5);
  //Okuma
  radio.startListening();
  if (radio.available())
  {
    radio.read(&stateButton2, sizeof(stateButton2));
    if (stateButton2 == LOW)
    {
      digitalWrite(pinLED, HIGH);
    }
    else
    {
      digitalWrite(pinLED, LOW);
    } 
  }
}