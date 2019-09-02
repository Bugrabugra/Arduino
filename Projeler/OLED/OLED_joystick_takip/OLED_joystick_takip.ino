#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int joyX = 0;
const int joyY = 1;

int valueX = 0;
int valueY = 0;

void setup()
{
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop()
{
  valueX = analogRead(joyX);
  valueX = map (valueX, 0, 1023, 0, 128);
  Serial.println("X: "); Serial.println(valueX);

  valueY = analogRead(joyY);
  valueY = map (valueY, 0, 1023, 0, 64);
  Serial.println("Y: "); Serial.println(valueY);

  delay(15);

  display.clearDisplay();

  display.drawPixel(valueX, valueY, WHITE);
  display.drawPixel(valueX - 1, valueY + 1, WHITE);
  display.drawPixel(valueX - 1, valueY, WHITE);
  display.drawPixel(valueX - 1, valueY - 1, WHITE);
  display.drawPixel(valueX, valueY + 1, WHITE);
  display.drawPixel(valueX, valueY - 1, WHITE);
  display.drawPixel(valueX + 1, valueY + 1, WHITE);
  display.drawPixel(valueX + 1, valueY, WHITE);
  display.drawPixel(valueX + 1, valueY - 1, WHITE);

  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.display();
}
