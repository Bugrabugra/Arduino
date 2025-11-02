#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AiEsp32RotaryEncoder.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    (-1)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*
connecting Rotary encoder

Rotary encoder side    MICROCONTROLLER side
-------------------    ---------------------------------------------------------------------
CLK (A pin)            any microcontroller intput pin with interrupt -> in this example pin 32
DT (B pin)             any microcontroller intput pin with interrupt -> in this example pin 21
SW (button pin)        any microcontroller intput pin with interrupt -> in this example pin 25
GND                    microcontroller GND
VCC                    microcontroller VCC (then set ROTARY_ENCODER_VCC_PIN -1)

***OR in case VCC pin is not free you can cheat and connect:***
VCC                    any microcontroller output pin - but set also ROTARY_ENCODER_VCC_PIN 25
                        in this example pin 25

*/
#if defined(ESP8266)
#define ROTARY_ENCODER_A_PIN D6
#define ROTARY_ENCODER_B_PIN D5
#define ROTARY_ENCODER_BUTTON_PIN D7
#else
#define ROTARY_ENCODER_A_PIN 2
#define ROTARY_ENCODER_B_PIN 3
#define ROTARY_ENCODER_BUTTON_PIN 4
#endif
#define ROTARY_ENCODER_VCC_PIN (-1) /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */



//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4

//instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(
  ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS,
  false
);


// Needle angle (0-359 degrees)
int angle = 0;

void rotary_loop() {
  if (rotaryEncoder.encoderChanged()) {
    angle = rotaryEncoder.readEncoder() % 360;
    if (angle < 0) angle += 360;
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize rotary encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup([] { rotaryEncoder.readEncoder_ISR(); });
  rotaryEncoder.setBoundaries(0, 359, true);
  rotaryEncoder.setAcceleration(0);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 not found"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  rotary_loop();

  // Reset angle if button is clicked
  if (rotaryEncoder.isEncoderButtonClicked()) {
    rotaryEncoder.reset(0);
    angle = 0;
  }

  display.clearDisplay();

  // Center coordinates
  int cx = SCREEN_WIDTH / 2;
  int cy = SCREEN_HEIGHT / 2;

  // Needle length
  int r = 25;

  // Convert angle to radians
  const float rad = (angle - 90) * 3.14159 / 180.0;

  // Needle endpoint
  int x = cx + r * cos(rad);
  int y = cy + r * sin(rad);

  // Draw circle
  display.drawCircle(cx, cy, r, SSD1306_WHITE);

  // Draw needle
  display.drawLine(cx, cy, x, y, SSD1306_WHITE);

  // Show angle value
  display.setCursor(10, 50); // Move text lower and a bit inside
  display.setTextSize(2); // Bigger text
  display.setTextColor(SSD1306_WHITE);
  display.print("Angle: ");
  display.print(angle);

  display.display();

  delay(20);
}
