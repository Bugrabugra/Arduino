// Libraries
#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

//Object declarations

SSD1306  display(0x3c, 0, 2);//0x3C being the usual address of the OLED

void setup() {
  Wire.pins(0, 2);// yes, see text
  Wire.begin(0,2);// 0=sda, 2=scl

  // Initialise the display.
  display.init();
  display.setFont(ArialMT_Plain_10);
}


void loop() {
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  String t = "Bugra";
  String t2 = "Burcu";
  display.drawString(0, 10, t);//
  display.drawString(0, 24, t2);
  // write the buffer to the display
  display.display();
  delay(10);
}
