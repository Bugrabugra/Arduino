#include <LiquidCrystal.h> 
#include <SoftwareSerial.h> 
#include <TinyGPS.h> 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float lat = 28.5458,lon = 77.1703; // create variable for latitude and longitude object  
SoftwareSerial gpsSerial(3,4);//rx,tx 

TinyGPS gps; // create gps object 

void setup()
{ 
  Serial.begin(9600); // connect serial 
  gpsSerial.begin(9600); // connect gps sensor 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
} 
void loop()
{ 
  while(gpsSerial.available())
  { // check for gps data 
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setCursor(0, 55);
    display.print("Bagli!");
    display.display();
    
    if(gps.encode(gpsSerial.read()))// encode gps data 
    {  
      gps.f_get_position(&lat,&lon); // get latitude and longitude 
      // display position 
 
      display.clearDisplay();
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(WHITE); // Draw white text
      display.cp437(true);         // Use full 256 char 'Code Page 437' font
      display.setCursor(0, 0);
      display.print("LAT:");
      display.setCursor(15, 0);
      display.print(lat);
      display.setCursor(0, 30);
      display.print("LAT:");
      display.setCursor(15, 30);
      display.print(lon);
      
      display.display();
    } 
  } 
  String latitude = String(lat,6); 
  String longitude = String(lon,6); 
  Serial.println(latitude+";"+longitude); 
  delay(1000); 
} 
