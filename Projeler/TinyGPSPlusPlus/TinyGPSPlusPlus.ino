#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Choose two Arduino pins to use for software serial
int RXPin = 2;
int TXPin = 3;

int GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup()
{
  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 0);
  display.print("Ekran baslatildi!");
  
  display.display();
  delay(5000);
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setCursor(0, 0);
    display.print("GPS algilanamadi!");
    
    display.display();
    
    while(true);
  }
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setCursor(0, 0);
    display.print("Latitude: ");
    display.setCursor(20, 0);
    display.print(gps.location.lat(), 6);
    display.setCursor(0, 20);
    display.print("Longitude: ");
    display.setCursor(20, 20);
    display.println(gps.location.lng(), 6);
    display.setCursor(0, 40);
    display.print("Altitude: ");
    display.setCursor(20, 40);
    display.println(gps.altitude.meters());
    
    display.display();
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setCursor(0, 0);
    display.print("Konum bulunamadi!");
    
    display.display();
  }
  delay(1000);
}
