#include <SPI.h>
#include "RF24.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RF24 radio(7, 8); //CE,CSN

byte addresses[][6] = {"1Node", "2Node"};

int msg[1];
int pinLaser = 5;
int pinLED = 2;
int adet = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(pinLaser, OUTPUT);
  pinMode(pinLED, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  radio.begin();

  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  radio.startListening();
}

void loop()
{
  digitalWrite(pinLaser, HIGH);

  if (radio.available())
  {
    while (radio.available())
    {
      radio.read(msg, 1);

      Serial.println(msg[0]);

      if (msg[0] == 0)
      {
        digitalWrite(pinLED, HIGH);

        adet = adet + 1;

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.cp437(true);
        display.setCursor(10, 20);
        display.print("Kisi: ");
        display.setCursor(80, 20);
        display.print(adet);

        display.display();
        delay(1000);
      }
      else
      {
        digitalWrite(pinLED, LOW);

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.cp437(true);
        display.setCursor(10, 20);
        display.print("Kisi: ");
        display.setCursor(80, 20);
        display.print(adet);

        display.display();
      }
    }
  }
}
