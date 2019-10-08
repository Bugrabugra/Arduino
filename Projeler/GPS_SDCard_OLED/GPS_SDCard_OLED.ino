#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

File myFile;
int RXPin = 2;
int TXPin = 3;
int GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() 
{
  //Initialize display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(5)) 
  {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
}

void loop() 
{
  unsigned int satellite_count = gps.satellites.value();
  float latitude = gps.location.lat();
  float longitude = gps.location.lng();
  unsigned int day_int = gps.date.day();
  unsigned int month_int = gps.date.month();
  unsigned int year_int = gps.date.year();

  char day_char[2];
  char month_char[2];
  char year_char[4];
  char myFile_char[8];

  itoa(day_int, day_char, 10);
  itoa(month_int, month_char, 10);
  itoa(year_int, year_char, 10);

  strcat(myFile_char, day_char);
  strcat(myFile_char, month_char);
  strcat(myFile_char, year_char);

  myFile = SD.open(myFile_char + ".TXT");



}