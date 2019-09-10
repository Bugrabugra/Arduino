#include <SD.h>

int pinCS = 53;
File youaremy_file;
File day_file;
long randNumber;
String list = "";
int next = 0;

void RandomInit()
{
  randomSeed(analogRead(0));
}

void PinsInit()
{
  pinMode(pinCS, OUTPUT);
}


void CardInit()
{
  SD.begin();
  youaremy_file = SD.open("youaremy.txt", FILE_WRITE);

  if (youaremy_file)
  {
    youaremy_file.close();
  }
}


void setup() 
{
  Serial.begin(9600);

  PinsInit();
  RandomInit();
  CardInit();
}

// void loop() 
// {
//   randNumber = random(4);
 
//   int recNum = 0; // We have read 0 records so far

//   youaremy_file = SD.open("youaremy.txt");

//   Serial.print("random number: ");
//   Serial.println(randNumber);

//   while (youaremy_file.available())
//   {
//     list = youaremy_file.readStringUntil('\r');
//     recNum++; // Count the record

//     if(recNum == randNumber)
//     {
//       Serial.println("aranilan deger");
//       Serial.println();
//     }
//   }
//   Serial.println(list);
//   youaremy_file.close();
// }

void loop()
{
  youaremy_file = SD.open("youaremy.txt");

  int index = 0;
  char stringArray[4];

  while ((next = youaremy_file.read()) != -1)
  {
    char nextChar = (char) next;
    if (nextChar == '\n')
    {
        stringArray[index] = '\0';
        Serial.println(stringArray[index]);
        index = 0;
    }
    else
    {
        stringArray[index] = nextChar;
        index += 1;
    }
  }
}