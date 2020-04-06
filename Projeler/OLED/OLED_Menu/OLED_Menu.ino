#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int pinButtonUp = 2;
int pinButtonDown = 3;
int pinButtonBack = 4;
int pinButtonEnter = 5;

int pinGreenLED = 6;
int pinBlueLED = 7;

int pinBuzzer = 8;

int valButtonDown = 0;
int valButtonUp = 0;
int valButtonEnter = 0;
int valButtonBack = 0;

int cursorPos = 0;
int numPage = 0;

int menuRowPixels = 15;
int rowsMenuMain = 4;
int rowsLEDler = 2;
int rowsGreenLED = 2;

char *menuMain[] = {"Araclar", "Uygulamalar", "Haritalar", "LEDler"};
char *menuLEDler[] = {"Green LED", "Blue LED"};
char *menuGreenLED[] = {"Green High", "Green Low"};

void setup()
{
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(pinButtonDown, INPUT_PULLUP);
  pinMode(pinButtonUp, INPUT_PULLUP);
  pinMode(pinButtonEnter, INPUT_PULLUP);
  pinMode(pinButtonBack, INPUT_PULLUP);
  pinMode(pinGreenLED, OUTPUT);
  pinMode(pinBlueLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
}

void loop()
{
  valButtonDown = digitalRead(pinButtonDown);
  valButtonUp = digitalRead(pinButtonUp);
  valButtonEnter = digitalRead(pinButtonEnter);
  valButtonBack = digitalRead(pinButtonBack);

  menu_populator(menuMain, rowsMenuMain);

  

  display.setCursor(0, cursorPos);
  display.print(">");

  display.display();
}

void buzzer()
{
  tone(pinBuzzer, 2000);
  delay(50);
  noTone(pinBuzzer);
  delay(50);
}

void menu_populator(char *menu_name_list[], int menuRowCount)
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.cp437(true);
  
  for (int i = 0; i < menuRowCount; i++)
  {
    display.setCursor(10, menuRowPixels * i);
    display.print(menu_name_list[i]);
  }

  if (valButtonDown == 0 && numPage == 0)
  {
    delay(200);
    cursorPos = cursorPos + menuRowPixels;
    buzzer();
    if (cursorPos > ((menuRowCount-1) * menuRowPixels))
    {
      cursorPos = 0;
    }
  }

  if (valButtonUp == 0 && numPage == 0)
  {
    delay(200);
    cursorPos = cursorPos - menuRowPixels;
    buzzer();
    if (cursorPos < 0)
    {
      cursorPos = ((menuRowCount-1) * menuRowPixels);
    }
  }
}

// void menu_walker(int rowNumber, int up_down, int page)
// {
//   if (up_down == 0)
//   {
//     if (valButtonDown == 0 && numPage == page)
//     {
//       delay(200);
//       cursorPos = cursorPos + menuRowPixels;
//       buzzer();
//       if (cursorPos > rowNumber * menuRowPixels)
//       {
//         cursorPos = 0;
//       }
//     }
//   }
  
//   else
//   {
//     if (valButtonUp == 0 && numPage == page)
//     {
//       delay(200);
//       cursorPos = cursorPos - menuRowPixels;
//       buzzer();
//       if (cursorPos < 0)
//       {
//         cursorPos = rowNumber * menuRowPixels;
//       }
//     }
//   } 
// }
