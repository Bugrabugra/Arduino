#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SSD1306.h"
#include <ESP8266HTTPClient.h>
// https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "credentials.h"

SSD1306  display(0x3c, 0, 2);//0x3C being the usual address of the OLED

const char ssid[] = WIFI_SSID_EV;
const char password[] = WIFI_PASSWORD_EV;
const char* host = "api.thingspeak.com";  //We read the data from this host                                   
const int httpPortRead = 80; 
const char* thinghttp_address = "/apps/thinghttp/send_request?api_key=ID01RLTWTE0EF82T";
String Data_Raw;
    
WiFiClient client;                                                  
HTTPClient http; 

void setup() {
  Wire.pins(0, 2);// yes, see text
  Wire.begin(0, 2);// 0=sda, 2=scl

  // Initialise the display.
  display.init();

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connecting to");
  display.drawString(0, 14, ssid);

  // write the buffer to the display
  display.display();
  delay(2000);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.clear();
    display.drawString(0, 0, "Connecting");
    display.display();
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connected");
  display.display();
}

void loop() {
  //Reading 1: Reading of cases
  if (http.begin(host, httpPortRead, thinghttp_address)) {
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {                
        Data_Raw = http.getString();   //Here we store the raw data string
        Serial.print("Value");  //I choosed to display it on the serial monitor to help you debug
        Serial.println(Data_Raw);
                                                 
        // To_remove = Data_Raw.indexOf(">");                      //I look for the position of this symbol ">"                                 
        // Data_Raw.remove(0,To_remove+1);                        //I remove it and everything that's before
        // To_remove = Data_Raw.indexOf("<");                     //I look for the position of this symbol ">"                                
        // Data_Raw.remove(To_remove,Data_Raw.length());          //I remove it and everything that's after

        // Cases=Data_Raw;
        // Serial.print("Cases: ");  //I choosed to display it on the serial monitor to help you debug
        // Serial.println(Cases);   

        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.drawString(0, 0, Data_Raw);
        display.display(); 

        delay(5000);                                           
                                                                                      
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();                                                                 
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
  
  delay(10000);

  while (WiFi.status() != WL_CONNECTED) { 
    WiFi.disconnect();                                                        
    delay(1000);                                                             
    
    WiFi.begin(ssid, password);                                              
    Serial.println("Reconnecting to WiFi..");       
    delay(10000);                                                             
  }
}


  
