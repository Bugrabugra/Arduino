// https://github.com/CheapskateProjects/GPSLogger/blob/master/GPSLogger.ino

#include <SD.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


// Pins used for communicating with GPS module
int RXPin = 2;
int TXPin = 3;

// Baud rate of your GPS module (usually 4800 or 9600)
int GPSBaud = 9600;

// How frequently should we save current location (milliseconds)
static const unsigned long frequency = 5000;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

// true when we have found location since last restart and file has been opened
boolean opened = false;

// current data file
File dataFile;

// file name
String fileName;

// timestamp of previous location capture. Used to check if we should save this location or skip it
unsigned long previous = 0;

int count = 0; 

void setup() 
{
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  if (!SD.begin(5)) 
  {
    while (true);
  }
}

void loop() 
{
  while (gpsSerial.available() > 0)
  {
    if (gps.encode(gpsSerial.read()))
    {
      logInfo();
    }
  }

  if (millis() > 10000 && gps.charsProcessed() < 10)
  {
    while(true);
  }
}

// Help function to pad 0 prefix when values < 10
void printIntValue(int value)
{
  if(value < 10)
  {
    dataFile.print(F("0"));
  }
  dataFile.print(value);
}

void logInfo()
{
  // Wait until we have location locked!
  if(!gps.location.isValid())
  {
    return;
  }

  if(!opened)
  {
    // When we first get something to log we take file name from that time
    fileName = "";
    fileName += gps.date.year();
    if(gps.date.month() < 10) fileName += "0";
    fileName += gps.date.month();
    if(gps.date.day() < 10) fileName += "0";
    fileName += gps.date.day();
    fileName += ".txt";
    opened = true;
  }

  if(millis() - previous > frequency)
  {
    previous = millis();
    // Write data row (dd.MM.yyyy HH:mm:ss lat,lon)
    dataFile = SD.open(fileName, FILE_WRITE);
    printIntValue(gps.date.day());
    dataFile.print(F("."));
    printIntValue(gps.date.month());
    dataFile.print(F("."));
    dataFile.print(gps.date.year());
    dataFile.print(F(" "));
    printIntValue(gps.time.hour());
    dataFile.print(F(":"));
    printIntValue(gps.time.minute());
    dataFile.print(F(":"));
    printIntValue(gps.time.second());
    dataFile.print(F(" "));
    dataFile.print(gps.location.lat(), 6);
    dataFile.print(F(","));
    dataFile.print(gps.location.lng(), 6);
    dataFile.println();
    dataFile.close();
  }
}