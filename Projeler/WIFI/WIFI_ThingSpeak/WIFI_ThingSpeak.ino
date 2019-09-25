#include <stdlib.h>

#define SSID "SONRASI_YOKTU"//your network name
#define PASS "BuuRA03045025"//your network password
//#define SSID "KAT3"//your network name
//#define PASS "UnV-2019!Wf++"//your network password
#define IP "184.106.153.149" // thingspeak.com
#define Baud_Rate 115200 //Another common value is 9600
#define DELAY_TIME 5000 //time in ms between posting data to ThingSpeak

//Can use a post also
String GET = "GET /update?api_key=571EEA6LUPEM9IHU&field1=";

//if you want to add more fields this is how
//String FIELD3 = "&field3=";

bool updated;

int pinButton = 3;
int pinLED = 4;
int valButton = 0;

//this runs once
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

//this runs over and over
void loop()
{
  valButton = digitalRead(pinButton);

  //update ThingSpeak channel with new values
  updated = updateLight(String(int(valButton)));

  //wait for delay time before attempting to post again
  digitalWrite(pinLED, HIGH);
  delay(DELAY_TIME);
  digitalWrite(pinLED, LOW);
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
  cmd += valButton;


  //continue to add data here if you have more fields such as a light sensor
  //cmd += FIELD3;
  //cmd += <field 3 value>

  cmd += "\r\n";

  //Use AT commands to send data
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if (Serial.find(">"))
  {
    //send through command to update values
    Serial.print(cmd);
  }
  else {
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
