#include <Arduino.h>

/*
  Hackman3D Joystick CAD Controller
  PAN / ORBIT toggle + axis invert
  makerworld.com / @Hackman3D
*/

#include <Mouse.h>
#include <Keyboard.h>

#define X A0
#define Y A1
#define SW 2

#define DEAD 25
#define SENS 100
#define TICK 10
#define LOCK 150

bool pan = 0, moving = 0;
int prev = HIGH;
unsigned long tToggle = 0, tTick = 0;

inline void apply() {
  if (!pan) Keyboard.press(KEY_LEFT_SHIFT);
  Mouse.press(MOUSE_MIDDLE);
}

inline void releaseAll() {
  Mouse.release(MOUSE_MIDDLE);
  Keyboard.release(KEY_LEFT_SHIFT);
}

void setup() {
  Keyboard.begin();
  Mouse.begin();
  pinMode(SW,INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();

  int b = digitalRead(SW);
  if (prev == HIGH && b == LOW && now - tToggle > LOCK) {
    tToggle = now;
    pan = !pan;
    if (moving) {
      releaseAll();
      apply();
    }
  }
  prev = b;


  if (now - tTick < TICK) return;
  tTick = now;

  int dx = (analogRead(X) - 512);
  int dy = -(analogRead(Y) - 512);

  if (abs(dx) > DEAD || abs(dy) > DEAD) {
    if (!moving) {
      moving = 1;
      apply();
    }
    Mouse.move(dx / SENS, -dy / SENS, 0);
  } else if (moving) {
    moving = 0;
    releaseAll();
  }
}
