#include <stdlib.h>


const char SSID[] = "SONRASI_YOKTU"; //your network name
const char PASS[] = "BuuRA03045025"; //your network password
// const char SSID[] = "KAT3"; //your network name
// const char PASS[] = "UnV-2019!Wf++"; //your network password
const char IP[] = "184.106.153.149"; // thingspeak.com
#define Baud_Rate 115200 //Another common value is 9600
#define DELAY_TIME 5000 //time in ms between posting data to ThingSpeak

//Can use a post also
String GET = "GET /channels/871881/feeds.json?results=1 HTTP/1.0";

bool updated;

int pinButton = 3;
int pinLED = 4;
int valButton = 0;

void setup()
{
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
  valButton = digitalRead(pinButton);

  //update ThingSpeak channel with new values
  updated = updateLight(String(int(valButton)));

  //wait for delay time before attempting to post again
  digitalWrite(pinLED, HIGH);
  delay(DELAY_TIME);
  digitalWrite(pinLED, LOW);

  // display.clearDisplay();
  // display.setTextSize(2);      // Normal 1:1 pixel scale
  // display.setTextColor(WHITE); // Draw white text
  // display.cp437(true);         // Use full 256 char 'Code Page 437' font
  // display.setCursor(0, 0);
  // display.print("Deger: ");
  // display.setCursor(30, 0);
  // display.print(XXXX);

  // display.display();
}

bool updateLight(String state)
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
  // cmd += "\r\n";
  // cmd += "Host: api.thingspeak.com:80";

  //continue to add data here if you have more fields such as a light sensor
  //cmd += FIELD3;
  //cmd += <field 3 value>

  cmd += "\r\n";
  cmd += "Host: api.thingspeak.com";
  cmd += "\r\n";
  cmd += "\r\n";

  //Use AT commands to send data
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  
  if (Serial.find(">"))
  {
    //send through command to update values
    Serial.print(cmd);
  }
  else 
  {
    Serial.println("AT+CIPCLOSE");
  }

  if (Serial.find("OK"))
  {
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


