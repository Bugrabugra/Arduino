#define DS3231_I2C_ADDRESS 0x68
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Convert decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

// Convert binary coded decimal to decimal numbers
byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

// Set the time
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);                     // set next input to start at the seconds register
  Wire.write(decToBcd(second));      // set seconds
  Wire.write(decToBcd(minute));      // set minutes
  Wire.write(decToBcd(hour));        // set hours
  Wire.write(decToBcd(dayOfWeek));   // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth));  // set date (1 to 31)
  Wire.write(decToBcd(month));       // set month
  Wire.write(decToBcd(year));        // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);  // set DS3231 register pointer to 00h
  Wire.endTransmission();

  // request seven bytes of data from DS3231 starting from register 00h
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

const unsigned char fish_bubbles[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
  0x70, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0x00, 0x00, 0x18, 0x07, 0x00, 0x00, 0x10, 0x0F, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00,
  0x00, 0xFF, 0x80, 0x00, 0xC3, 0xFF, 0xF0, 0x00, 0xC7, 0xFF, 0xFC, 0x07, 0x0F, 0xFF, 0xFF, 0x9E,
  0x1D, 0xFF, 0xFF, 0xFC, 0x1D, 0xFF, 0xFF, 0xFC, 0x1F, 0xFF, 0xFF, 0xFC, 0x1F, 0xFF, 0xFF, 0xFE,
  0x0F, 0xFF, 0xFF, 0x9E, 0x07, 0xFF, 0xFE, 0x07, 0x03, 0xFF, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00,
  0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int pinLED = 10;
int pinButton = 11;
byte valLEDState = HIGH;

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Wire.begin();
  Serial.begin(9600);
  // set the initial time here: ONLY DO THIS ONCE!
  // DS3231 seconds, minutes, hours, day, date, month, year
  // Note Sunday is 1, Monday is 2 etc
  //setDS3231time(0, 44, 23, 1, 31, 03, 19);
}

void loop() {
  resetValues();
  ledMorningOn();
  ledButtonOff();
}

void resetValues() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  if ((hour == 7) && (minute == 40)) {
    valLEDState = HIGH;
  }
}

void ledMorningOn() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  if ((hour > 7) && (valLEDState == HIGH)) {
    digitalWrite(pinLED, HIGH);

    display.clearDisplay();
    display.setTextSize(3);       // Normal 1:1 pixel scale
    display.setTextColor(WHITE);  // Draw white text
    display.cp437(true);          // Use full 256 char 'Code Page 437' font

    display.setCursor(15, 33);
    display.drawBitmap(15, 33, fish_bubbles, 32, 32, 1);

    display.setCursor(5, 8);

    if (hour < 10) {
      display.print("0");
    }
    display.print(hour);

    display.print(':');

    if (minute < 10) {
      display.print("0");
    }
    display.print(minute);

    display.setTextSize(2);
    display.setCursor(100, 8);

    if (second < 10) {
      display.print("0");
    }
    display.print(second);

    display.setCursor(65, 45);
    display.setTextSize(2);

    if (dayOfMonth < 10) {
      display.print("0");
    }
    display.print(dayOfMonth);

    display.print('/');

    if (month < 10) {
      display.print("0");
    }
    display.print(month);

    display.display();
  }
}

void ledButtonOff() {
  if (digitalRead(pinButton) == 0) {
    digitalWrite(pinLED, LOW);
    valLEDState = LOW;

    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

    // retrieve data from DS3231
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
                   &year);

    display.clearDisplay();
    display.setTextSize(3);       // Normal 1:1 pixel scale
    display.setTextColor(WHITE);  // Draw white text
    display.cp437(true);          // Use full 256 char 'Code Page 437' font

    display.setCursor(15, 33);
    display.drawBitmap(15, 33, fish_bubbles, 32, 32, 1);

    display.setCursor(5, 8);

    if (hour < 10) {
      display.print("0");
    }
    display.print(hour);

    display.print(':');

    if (minute < 10) {
      display.print("0");
    }
    display.print(minute);

    display.setTextSize(2);
    display.setCursor(100, 8);

    if (second < 10) {
      display.print("0");
    }
    display.print(second);

    display.setCursor(65, 45);
    display.setTextSize(2);

    if (dayOfMonth < 10) {
      display.print("0");
    }
    display.print(dayOfMonth);

    display.print('/');

    if (month < 10) {
      display.print("0");
    }
    display.print(month);

    display.display();
  }
}