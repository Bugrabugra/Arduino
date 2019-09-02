// Thingspeak  
String myAPIkey = "BAFE28CFOS4W2F7I";

#include <SoftwareSerial.h>
#include <SimpleDHT.h>

SoftwareSerial ESP8266(7, 6); // Rx,  Tx
															/* DHT SENSOR SETUP */

int pinDHT11 = 13;
SimpleDHT11 dht11(pinDHT11);

// Convert decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
	return ((val / 10 * 16) + (val % 10));
}

// Convert binary coded decimal to decimal numbers
byte bcdToDec(byte val)
{
	return ((val / 16 * 10) + (val % 16));
}

// Thermometrenin bitmapi
const unsigned char thermo[] PROGMEM =
{
	0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x06, 0x0F, 0x81, 0xE0,
	0x07, 0x80, 0x1B, 0xF8, 0x07, 0xDF, 0x9B, 0xB8, 0x07, 0x1F, 0xCB, 0x58, 0x02, 0x7F, 0xE2, 0x58,
	0x00, 0xFF, 0xF2, 0x58, 0x01, 0xFF, 0xFA, 0x58, 0x01, 0xFF, 0xFA, 0x58, 0x3B, 0xFF, 0xFA, 0x58,
	0x7B, 0xFF, 0xFA, 0x58, 0x7B, 0xFF, 0xFA, 0x58, 0x7B, 0xFF, 0xFA, 0x58, 0x09, 0xFF, 0xFA, 0x18,
	0x01, 0xFF, 0xFA, 0x18, 0x01, 0xFF, 0xFA, 0x18, 0x00, 0x7F, 0xF2, 0x18, 0x06, 0x3F, 0xE2, 0x18,
	0x07, 0x9F, 0x86, 0x1C, 0x07, 0xC0, 0x0E, 0x1E, 0x07, 0x0F, 0x9E, 0x06, 0x00, 0x0F, 0x9C, 0x07,
	0x00, 0x07, 0x38, 0x1B, 0x00, 0x06, 0x38, 0x1B, 0x00, 0x02, 0x18, 0x1B, 0x00, 0x00, 0x1C, 0x37,
	0x00, 0x00, 0x1C, 0x0E, 0x00, 0x00, 0x0F, 0xFE, 0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x01, 0xE0
};

// Nemin bitmapi
const unsigned char humi[] PROGMEM =
{
	0x01, 0x80, 0x03, 0xC0, 0x07, 0xE0, 0x09, 0xF0, 0x09, 0xF0, 0x19, 0xF8, 0x17, 0xF8, 0x37, 0xFC,
	0x77, 0xFC, 0x6F, 0xFC, 0x6F, 0xFC, 0x6F, 0xFE, 0x9F, 0xFE, 0xBF, 0xFE, 0xFF, 0xFE, 0x9F, 0xFE,
	0xFF, 0xFE, 0x7F, 0xFE, 0x7F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x0F, 0xF0,
};


boolean relay1_st = false;
boolean relay2_st = false;
unsigned char check_connection = 0;
unsigned char times_check = 0;
boolean error;

void setup()
{
	Serial.begin(9600);
	ESP8266.begin(9600);
	startTime = millis();
	ESP8266.println("AT+RST");
	delay(2000);
	Serial.println("Connecting to Wifi");

	while (check_connection == 0)
	{
		Serial.print(".");
		ESP8266.print("AT+CWJAP=\"KAT2S 2\",\"UnV-2019!Wf++\"\r\n");
		ESP8266.setTimeout(5000);
		if (ESP8266.find("WIFI CONNECTED\r\n") == 1)
		{
			Serial.println("WIFI CONNECTED");
			break;
		}
		times_check++;
		if (times_check>3)
		{
			times_check = 0;
			Serial.println("Trying to Reconnect..");
		}
	}
}

void printTemperature()
{
	byte temperature = 0;
	byte humidity = 0;
	int err = SimpleDHTErrSuccess;

	if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
	{
		display.print("DHT11 sensörü okumasýnda hata oluþtu, Hata=");
		display.print(err);
		return;
	}

	display.clearDisplay();
	display.setTextSize(1.4);      // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.cp437(true);         // Use full 256 char 'Code Page 437' font

	display.drawBitmap(0, 1, thermo, 32, 32, 1);
	display.drawBitmap(6, 36, humi, 16, 22, 1);

	display.setCursor(38, 13);
	display.print("Sicaklik: "); display.print((int)temperature); display.print(" *C ");
	display.setCursor(38, 41);
	display.print("Nem: "); display.print((int)humidity); display.print(" H ");

	display.display();
	delay(500);
}


void loop()
{
	waitTime = millis() - startTime;
	if (waitTime > (writingTimer * 1000))
	{
		printTemperature();
		writeThingSpeak();
		startTime = millis();
	}
}



void writeThingSpeak(void)
{
	startThingSpeakCmd();
	// preparacao da string GET
	String getStr = "GET /update?api_key=";
	getStr += myAPIkey;
	getStr += "&field1=";
	getStr += String((int)temperature);
	getStr += "&field2=";
	getStr += String((int)humidity);
	getStr += "\r\n\r\n";
	GetThingspeakcmd(getStr);
}

void startThingSpeakCmd(void)
{
	ESP8266.flush();
	String cmd = "AT+CIPSTART=\"TCP\",\"";
	cmd += "184.106.153.149"; // api.thingspeak.com IP address
	cmd += "\",80";
	ESP8266.println(cmd);
	Serial.print("Start Commands: ");
	Serial.println(cmd);

	if (ESP8266.find("Error"))
	{
		Serial.println("AT+CIPSTART error");
		return;
	}
}

String GetThingspeakcmd(String getStr)
{
	String cmd = "AT+CIPSEND=";
	cmd += String(getStr.length());
	ESP8266.println(cmd);
	Serial.println(cmd);

	if (ESP8266.find(">"))
	{
		ESP8266.print(getStr);
		Serial.println(getStr);
		delay(500);
		String messageBody = "";
		while (ESP8266.available())
		{
			String line = ESP8266.readStringUntil('\n');
			if (line.length() == 1)
			{
				messageBody = ESP8266.readStringUntil('\n');
			}
		}
		Serial.print("MessageBody received: ");
		Serial.println(messageBody);
		return messageBody;
	}
	else
	{
		ESP8266.println("AT+CIPCLOSE");
		Serial.println("AT+CIPCLOSE");
	}
}