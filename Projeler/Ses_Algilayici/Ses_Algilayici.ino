int Led = 13;       // define LED Interface
int buttonpin = 3;  // define D0 Sensor Interface
int val = 0;        // define numeric variables val

void setup() {
  pinMode(Led, OUTPUT);       // define LED as output interface
  pinMode(buttonpin, INPUT);  // output interface D0 is defined sensor
  Serial.begin(9600);
}

void loop() {
  val = digitalRead(buttonpin);  // digital interface will be assigned a value of pin 3 to read val
  if (val == HIGH)               // When the sound detection module detects a signal, LED flashes
  {
    digitalWrite(Led, HIGH);
    Serial.println("Ses");
    delay(1000);
  } else {
    digitalWrite(Led, LOW);
    Serial.println("Ses yok");
    delay(1000);
  }
}
