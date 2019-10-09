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

int satellite_count = 0;
float latitude = 0;
float longitude = 0;
int day_int = 0;
int month_int = 0;
int year_int = 0;

String day_str = "";
String month_str = "";
String year_str = "";
String date_time = "";
String satellite = "";

int count = 0; 

void setup() 
{
  //Initialize display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  if (!SD.begin(5)) 
  {
    while (1);
  }
}

void loop() 
{
  satellite_count = gps.satellites.value();
  latitude = gps.location.lat();
  longitude = gps.location.lng();
  day_int = gps.date.day();
  month_int = gps.date.month();
  year_int = gps.date.year();

  // satellite_count = 8;
  // latitude = 41.233333;
  // longitude = 28.654564;
  // day_int = 9;
  // month_int = 10;
  // year_int = 2019;

  if (day_int < 10)
  {
    day_str = "0" + String(day_int);
  }
  else 
  {
    day_str = String(day_int);
  }

  if (month_int < 10)
  {
    month_str = "0" + String(month_int);
  }
  else 
  {
    month_str = String(month_int);
  }

  year_str = String(year_int);
  date_time = day_str + month_str + year_str;
  
  myFile = SD.open("GPS.TXT", FILE_WRITE);
  myFile.println(String(count) + "-" + String(latitude) + "-" + String(longitude) + "-" + String(date_time) + "-" + String(satellite_count));
  myFile.close();
  Serial.println(String(count) + "-" + String(latitude) + "-" + String(longitude) + "-" + String(date_time) + "-" + String(satellite_count));
  delay(10000);
  count += 1;
}