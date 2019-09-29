#include <SoftwareSerial.h>
SoftwareSerial softSerial(8, 9); // RX, TX

void setup() 
{
  uint32_t baud = 115200;
  Serial.begin(baud);
  softSerial.begin(baud);
  Serial.print("SETUP!! @");
  Serial.println(baud);
  String cmd = "GET /channels/871881/feeds.json?results=1 HTTP/1.0\r\nHost: api.thingspeak.com\r\n\r\n";
  Serial.println(cmd.length());
}

void loop() 
{
    while(softSerial.available() > 0) 
    {
      char a = softSerial.read();
      if(a == '\0')
        continue;
      if(a != '\r' && a != '\n' && (a < 32))
        continue;
      Serial.print(a);
    }
    
    while(Serial.available() > 0)
    {
      char a = Serial.read();
      Serial.write(a);
      softSerial.write(a);
    }
}
