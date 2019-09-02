int pin_pot = A0;
byte okunan_deger = 0;

void setup()
{
  Serial.begin(9600);
  okunan_deger = analogRead(pin_pot);
}

void loop()
{
  Serial.print("Okunan değer: ");
  Serial.println(okunan_deger);
  Serial.println("------------");
  delay(1000);
}
