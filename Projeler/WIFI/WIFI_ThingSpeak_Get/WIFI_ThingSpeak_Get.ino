#include <ArduinoJson.h>
#include <stdlib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define MAX_STRING_LENGTH 10

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

// const char SSID[] = "SONRASI_YOKTU"; //your network name
// const char PASS[] = "BuuRA03045025"; //your network password
const char SSID[] = "KAT3"; //your network name
const char PASS[] = "UnV-2019!Wf++"; //your network password
const char IP[] = "184.106.153.149"; // thingspeak.com
#define Baud_Rate 115200 //Another common value is 9600
#define DELAY_TIME 5000 //time in ms between posting data to ThingSpeak

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Can use a post also
String GET = "GET /channels/871881/feeds.json?results=1";

bool updated;

int pinButton = 3;
int pinLED = 4;
int valButton = 0;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(pinLED, OUTPUT);
  
  Serial.begin(Baud_Rate);
  Serial.println("AT");

  delay(5000);

  if (Serial.find("OK"))
  {
    //connect to your wifi netowork
    bool connected = connectWiFi();
  }
}

void loop()
{
  //update ThingSpeak channel with new values
  updated = getLight();

  //wait for delay time before attempting to post again
  digitalWrite(pinLED, HIGH);
  delay(DELAY_TIME);
  digitalWrite(pinLED, LOW);
}

void printResponse() {
  while (Serial.available()) {
    Serial.println(Serial.readStringUntil('\n')); 
  }
}


bool getLight()
{
  //initialize your AT command string
  String cmd = "AT+CIPSTART=\"TCP\",\"";

  //add IP address and port
  cmd += IP;
  cmd += "\",80";

  //connect
  Serial.println(cmd);
  delay(2000);
  if (Serial.find("Error"))
  {
    return false;
  }

  //build GET command, ThingSpeak takes Post or Get commands for updates, I use a Get
  cmd = GET;
  cmd += "\r\n";

  //Use AT commands to send data
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());

  if (Serial.find(">"))
  {
    //send through command to update values
    Serial.print(cmd);



      // const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 240;
      // DynamicJsonDocument doc(capacity);

      // const char* json = Serial.read();

      // deserializeJson(doc, json);

      // JsonObject feeds_0 = doc["feeds"][0];
      // const char* feeds_0_created_at = feeds_0["created_at"]; // "2019-09-26T06:18:14Z"
      // int feeds_0_entry_id = feeds_0["entry_id"]; // 66
      // const char* feeds_0_field1 = feeds_0["field1"]; // "1"

      // display.clearDisplay();
      // display.setTextSize(3);      // Normal 1:1 pixel scale
      // display.setTextColor(WHITE); // Draw white text
      // display.cp437(true);         // Use full 256 char 'Code Page 437' font
      // display.setCursor(0, 0);
      // display.print(feeds_0_field1);
      // display.display(); 
  }
  else {
    Serial.println("AT+CIPCLOSE");
  }

  if (Serial.find("OK"))
  {
    static char responseBuffer[1024]; // Buffer for received data

    while (Serial.available()) 
    {                         // Now receive the data
      String line = Serial.readStringUntil('\n');
      if ( line.indexOf('{',0) >= 0 ) 
      {                  // Ignore data that is not likely to be JSON formatted, so must contain a '{'
        Serial.println(line);                            // Show the text received
        line.toCharArray(responseBuffer, line.length()); // Convert to char array for the JSON decoder
      }
    }

    Serial.println(responseBuffer);
    //success! Your most recent values should be online.
    return true;
  }
  else
  {
    return false;
  }
}

boolean connectWiFi()
{
  //set ESP8266 mode with AT commands
  Serial.println("AT+CWMODE=1");
  delay(2000);

  //build connection command
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";

  //connect to WiFi network and wait 5 seconds
  Serial.println(cmd);
  delay(5000);

  //if connected return true, else false
  if (Serial.find("OK"))
  {
    Serial.println("Internete baglandi!");
    return true;
  }
  else
  {
    return false;
  }
}

