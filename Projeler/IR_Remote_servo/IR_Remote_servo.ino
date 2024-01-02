#include <IRremote.h>
#include <Servo.h>

const int RECV_PIN = 4;
const int SERVO_PIN = 8;

IRrecv irrecv(RECV_PIN);
decode_results results;

int pos = 90;
unsigned long lastCode;

Servo myservo;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  myservo.attach(SERVO_PIN);
  myservo.write(pos);
}

void loop() {
  if (irrecv.decode(&results)) {
    if (results.value = 0xFFFFFFFF) {
      results.value = lastCode;
    }

    if (results.value = 0xFF22DD) {
      lastCode = results.value;
      pos += 2;
      if (pos > 180) {
        pos = 180;
      }
      myservo.write(pos);
    }

    if (results.value = 0xFF02FD) {
      lastCode = results.value;
      pos -= 2;
      if (pos < 0) {
        pos = 0;
      }
      myservo.write(pos);
    }

    delay(30);

    irrecv.resume();
  }
}
