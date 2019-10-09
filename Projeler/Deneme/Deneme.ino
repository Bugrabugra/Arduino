int satellite_count = 8;
float latitude = 40.984512;
float longitude = 29.133350;
int day_int = 9;
int month_int = 10;
int year_int = 2019;

String day_str = "";
String month_str = "";
String year_str = "";
String file_name = "";

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if (day_int < 10)
  {
    day_str = "0" + String(day_int);
  }
  else 
  {
    day_str = String(day_int);
  }

  if (month_int < 10)
  {
    month_str = "0" + String(month_int);
  }
  else 
  {
    month_str = String(month_int);
  }

  year_str = String(year_int);

  file_name = day_str + month_str + year_str;

  Serial.print("day: ");
  Serial.println(day_str);
  Serial.print("month: ");
  Serial.println(month_str);
  Serial.print("year: ");
  Serial.println(year_str);
  Serial.print("file name: ");
  Serial.println(file_name);

  delay(10000);
}