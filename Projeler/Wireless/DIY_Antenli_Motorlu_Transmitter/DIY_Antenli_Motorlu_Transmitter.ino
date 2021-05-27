// 3 Channel Transmitter | 3 Kanal Verici
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Match key | Eşleşme anahtarı
const uint64_t pipeOut = 0xE9E8F0F0E1LL;   

// Select CE,CSN pin | CE ve CSN pinlerin seçimi
RF24 radio(9, 10);                         

struct Signal {
  //*** byte throttle;
  byte steering;
  //*** byte aux1;
};
  
Signal data;

void ResetData() {
  //*** data.throttle = 127;     // Motor stop | Motor Kapalı (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.steering = 127;    //  Center | Merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
  //*** data.aux1 = 0;          //  (Signal lost position | sinyal kesildiğindeki pozisyon)
}

void setup() {
  Serial.begin(9600);
  //Start everything up
  radio.begin();
  radio.openWritingPipe(pipeOut);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();             //start the radio comunication for Receiver | Verici olarak sinyal iletişimi başlatılıyor
  ResetData();
}

// Joystick center and it's borders | Joystick merkez ve sınırları
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse) {
  val = constrain(val, lower, upper);
  
  if ( val < middle ) {
    val = map(val, lower, middle, 0, 128);
  } else {
    val = map(val, middle, upper, 128, 255);
  }
  
  return ( reverse ? 255 - val : val );
}

void loop() {
  // Control Stick Calibration | Kumanda Kol Kalibrasyonları
  // Setting may be required for the correct values of the control levers. | Kontrol kollarının doğru değerleri için ayar gerekebilir.
  
  data.steering = mapJoystickValues( analogRead(A0), 200, 460, 650, true );    // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler
  //*** data.throttle = mapJoystickValues( analogRead(A1), 200, 440,670, true );    // "true" or "false" for signal direction | "true" veya "false" sinyal yönünü belirler
  //*** data.aux1 = digitalRead(4); 
  radio.write(&data, sizeof(Signal));
  Serial.println(data.steering);
}