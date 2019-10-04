char bir[23] = "AT+CIPSTART=\"TCP\",\"";
char iki[19] = "api.thingspeak.com";
char birlesmis[48] = "";

void setup()
{
  Serial.begin(9600);
  strcat(birlesmis, bir);
  strcat(birlesmis, iki);
  char uc[6] = "\",80";
  strcat(birlesmis, uc);
}

void loop()
{
  delay(1000);
  Serial.println(birlesmis);
  Serial.println(sizeof(birlesmis));
}