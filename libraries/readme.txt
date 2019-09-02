int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
int LEDpin = 11;          // connect Red LED to pin 11 (PWM pin)
int LEDbrightness;        // 
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
}
 
void loop(void) {
  photocellReading = analogRead(photocellPin);  
 
  Serial.print("Analog reading = ");
  Serial.println(photocellReading);     // the raw analog reading
  
  LEDbrightness = 1 / (photocellReading / 1024 * 256)
  
  digitalWrite(LEDpin, LEDbrightness)
  
}
  
  
  
  -------------------------------
  
  
 const int pinLED = 9;
 const int pinPotans = A0;
 
 int sensorValue = 0;
 int outputValue = 0;
 
 void setup() {
	 
	 pinMode(pinLED, OUTPUT);
	 pinMode(pinPotans, INPUT);
	 
 }
 
 void loop(){
	 
	 sensorValue = analogRead(pinPotans);
	 outputValue = map(pinPotans, 0,1023,0,255);
	 
 }