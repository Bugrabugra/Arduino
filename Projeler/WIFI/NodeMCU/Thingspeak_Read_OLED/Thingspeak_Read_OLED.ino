#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//----------------  Fill in your credentails   ---------------------
// char ssid[] = "KAT3";     // your network SSID (name) 
// char pass[] = "UnV-2019!Wf++"; // your network password
char ssid[] = "SONRASI_YOKTU";     // your network SSID (name) 
char pass[] = "BuuRA03045025"; // your network password
//------------------------------------------------------------------

WiFiClient  client;

unsigned long buttonChannelNumber = 747521;
unsigned int buttonFieldNumber = 7;

unsigned long lightChannelNumber = 739580;
const char * myLightReadAPIKey = "36EGT7ZHVS9WTB9F";
unsigned int lightFieldNumber = 1; 

void setup() 
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  WiFi.mode(WIFI_STA);  
  ThingSpeak.begin(client);
}

void loop() 
{
  int statusCode = 0;
 
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
 }

  // Read in field 4 of the public channel recording the temperature
  float button = ThingSpeak.readFloatField(buttonChannelNumber, buttonFieldNumber);  

  // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200)
  {
    Serial.println("Light at MathWorks HQ: " + String(button));
  }
  else
  {
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }
  
  delay(15000); // No need to read the temperature too often.

  // Read in field 1 of the private channel which is a counter  
  long light = ThingSpeak.readLongField(lightChannelNumber, lightFieldNumber, myLightReadAPIKey);  

    // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200)
  {
    Serial.println("Light: " + String(light));
  }
  else
  {
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }

    display.clearDisplay();
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setCursor(0, 0);
    display.print("Value:");
    display.setTextSize(3); 
    display.setCursor(0, 25);
    display.print(button);

    display.display();
    delay(15000); // No need to read the counter too often.
}
