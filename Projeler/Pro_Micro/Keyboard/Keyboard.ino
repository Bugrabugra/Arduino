#include <Keyboard.h>

int buttonPin = 3; // Set a button to any pin

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP); // Set the button as an input
}

void loop()
{
  if (digitalRead(buttonPin) == 0) // if the button goes low
  {
    //Keyboard.write('z');  // send a 'z' to the computer via Keyboard HID
    Keyboard.println("Hello world");
    delay(1000); // delay so there aren't a kajillion z's
  }
}