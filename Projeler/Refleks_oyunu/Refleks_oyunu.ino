#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int pinBlue = 9;    //Mavi oyuncu
const int pinRed = 8;     // Kırmızı oyuncu
const int pinLed = 12;    //Başlama LEDi
const int pinStart = 13;  //Başlama düğmesi

int buttonStateBlue = 0;  //Mavi oyuncu anlık düğme değeri
int buttonStateRed = 0;   //Kırmızı oyuncu anlık düğme değeri

int lastButtonStateBlue = 0;  //Mavi oyuncu son durumu
int lastButtonStateRed = 0;   //Kırmızı oyuncu son durumu

int scoreBlue = 0;  //Mavi oyuncu puanı
int scoreRed = 0;   //Kırmızı oyuncu puanı

boolean bothDone = false;        //Her iki oyuncu da oyununu oynadı mı?
boolean gameOn = false;          //Oyun devam ediyor mu etmiyor mu?
boolean startButtonState = LOW;  //Başlangıç düğmesi durumu
boolean blueDone = false;        //Mavi oynadı mı?
boolean redDone = false;         //Kırmızı oynadı mı?
boolean welcomeMsg = false;      //Başlangıç mesajı gösterildi mi?

long randomTime;   //Oyunun başlamasıyla ışığın ikinci kez yanması arasında geçen random süre
long startTime;    //Oyunun başladığı an
long endTimeBlue;  //Mavi oyuncunun düğmesine bastığı an
long endTimeRed;   //Kırmızı oyuncunun düğmesine bastığı an

float finalTimeBlue;  //Oyunun başlamasından sonra mavi oyuncunun düğmesine basmasına kadar geçen süre
float finalTimeRed;   //Oyunun başlamasından sonra kırmızı oyuncunun düğmesine basmasına kadar geçen süre
float winningTime;    //Oyunun başlamasından, kazanan oyuncunun süresine kadar geçen süre

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(pinBlue, INPUT_PULLUP);
  pinMode(pinRed, INPUT_PULLUP);
  pinMode(pinStart, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
}

void loop() {
  if (welcomeMsg == false) {
    display.clearDisplay();
    display.setTextSize(1);       // Normal 1:1 pixel scale
    display.setTextColor(WHITE);  // Draw white text
    display.cp437(true);          // Use full 256 char 'Code Page 437' font
    display.setCursor(0, 0);
    display.print("Dugmeye basiniz");
    display.setCursor(0, 20);
    display.print("Skor");
    display.setCursor(0, 40);
    display.print("Mavinin puani: ");
    display.print(scoreBlue);
    display.setCursor(0, 50);
    display.print("Kirmizinin puani: ");
    display.print(scoreRed);

    display.display();

    welcomeMsg = true;
  }

  startButtonState = digitalRead(pinStart);  //Oyunun başlaması için başlama düğmesinin durumunu oku

  if (startButtonState == LOW && gameOn == false) {
    Random();
  }
}

void Random() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Hazir olun!");
  display.display();

  randomTime = random(4, 10);
  randomTime = randomTime * 1000;

  digitalWrite(pinLed, HIGH);  //İlk ışık yakılıyor. Sönüp tekrar yandığı an oyun başlayacak
  delay(1000);
  digitalWrite(pinLed, LOW);
  delay(randomTime);
  digitalWrite(pinLed, HIGH);

  startGame();
}

void startGame() {
  gameOn = true;               //Bir oyunun oynanmakta olduğunu belirtiyor
  startTime = millis();        //Oyunun başladığı an kaydediliyor
  digitalWrite(pinLed, HIGH);  //Oyunun başladığını gösteren LED'in yanışı

  while (blueDone == false || redDone == false) {
    buttonStateBlue = digitalRead(pinBlue);
    buttonStateRed = digitalRead(pinRed);

    if (buttonStateBlue == LOW && blueDone == false) {
      endTimeBlue = millis();
      blueDone = true;
    }

    if (buttonStateRed == LOW && redDone == false) {
      endTimeRed = millis();
      redDone = true;
    }
  }

  endGame();
}

void endGame() {
  digitalWrite(pinLed, LOW);  //Oyunun bittiğini gösterir

  finalTimeBlue = (endTimeBlue - startTime);  //Mavinin hızı
  finalTimeRed = (endTimeRed - startTime);    //Kırmızının hızı

  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Mavinin hizi: ");
  display.print(finalTimeBlue / 1000);
  display.setCursor(0, 40);
  display.print("Kirmizinin hizi: ");
  display.print(finalTimeRed / 1000);
  display.display();

  delay(4000);

  if (endTimeBlue < endTimeRed)  //Mavi kazandıysa
  {
    winningTime = (endTimeRed - startTime) - (endTimeBlue - startTime);
    scoreBlue = scoreBlue + 1;

    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Mavi su hizla kazandi");
    display.setCursor(0, 40);
    display.print(winningTime / 1000);
    display.print("saniye");
    display.display();
    delay(3000);
  } else  //Kırmızı kazandıysa
  {
    winningTime = (endTimeBlue - startTime) - (endTimeRed - startTime);
    scoreRed = scoreRed + 1;

    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Kirmizi su hizla kazandi");
    display.setCursor(0, 40);
    display.print(winningTime / 1000);
    display.print(" saniye");
    display.display();
    delay(5000);
  }

  // Bütün değişkenlere restart atılır
  buttonStateBlue = 0;
  buttonStateRed = 0;
  lastButtonStateBlue = 0;
  lastButtonStateRed = 0;
  bothDone = false;
  gameOn = false;
  startButtonState = LOW;
  blueDone = false;
  redDone = false;
  randomTime = 0;
  startTime = 0;
  endTimeBlue = 0;
  endTimeRed = 0;
  finalTimeBlue = 0;
  finalTimeRed = 0;
  winningTime = 0;
  welcomeMsg = false;
}
