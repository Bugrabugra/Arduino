#include <stdlib.h>
#include <SimpleDHT.h>

const char SSID[] = "SONRASI_YOKTU"; //your network name
const char PASS[] = "BuuRA03045025"; //your network password
// const char SSID[] = "KAT3"; //your network name
// const char PASS[] = "UnV-2019!Wf++"; //your network password
const char IP[] = "184.106.153.149"; // thingspeak.com
#define Baud_Rate 115200 //Another common value is 9600
#define DELAY_TIME 5000 //time in ms between posting data to ThingSpeak

//Can use a post also
String GET = "GET /update?api_key=BAFE28CFOS4W2F7I&field1=";
String FIELD2 = "&field2=";

//if you want to add more fields this is how
//String FIELD3 = "&field3=";

bool updated;

int pinDHT11 = 13;
SimpleDHT11 dht11(pinDHT11);

//this runs once
void setup()
{
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
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    return;
  }

  Serial.println(int(temperature));
  Serial.println(int(humidity));

  //update ThingSpeak channel with new values
  updated = updateTemp(String(int(temperature)), String(int(humidity)));

  //wait for delay time before attempting to post again
  delay(DELAY_TIME);
}

bool updateTemp(String tenmpF, String humid) 
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
  cmd += tenmpF;
  cmd += FIELD2;
  cmd += humid;

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
    Serial.println("Baglandi");
    return true;
  }
  else 
  {
    return false;
  }
}
