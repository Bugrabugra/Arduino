#include <SPI.h>
#include <SD.h>

File myFile;
long randNumber;
String myText = "";

void RandomInit()
{
  randomSeed(analogRead(0));
}


void setup() 
{
  RandomInit();
  
  randNumber = random(101, 104);
  Serial.begin(9600);
  SD.begin();

  myFile = SD.open(String(randNumber) + ".TXT");
  if (myFile) 
  {
    while (myFile.available()) 
    {
      myText += (char)myFile.read();
    }
    myFile.close();
  } 
  else 
  {
    Serial.println("error opening test.txt");
  }
}

void loop() 
{
  Serial.println(myText);
  delay(2000);
}


