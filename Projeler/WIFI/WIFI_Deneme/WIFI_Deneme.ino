#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ThingerWifi.h>

// thinger.io config
#define USER_ID "Buura"
#define DEVICE_ID "1"
#define DEVICE_CREDENTIAL "NvYF6k3gI!IV"
#define ALARM_ENDPOINT "your_alarm_endpoint"

// wifi config
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_WPA2_PASSWORD "your_wifi_password"

// sensor/led pins
#define SENSOR_PIN D1
#define LED_PIN D2

ThingerWifi thing(USER_ID, DEVICE_ID, DEVICE_CREDENTIAL);

typedef enum WaterState{
  WATER,
  START_DETECTING_NO_WATER,
  NO_WATER
};

WaterState waterState = WATER;          // current water state
bool showLedAlarm = true;               // control led alarm on/off
unsigned long ledAlarmInterval = 500;   // led alam blink frequency
unsigned long previousMillis = 0;       // util for hysteresis and blink
unsigned long timeWithoutWater = 0;     // for hysteresis time
unsigned long hysteresisTime = 60000;   // one minute
int ledState = LOW;                     // current led alarm state

void setup() {
  // set pinmode for led alarm and humidity reader
  pinMode(SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // add wifi connection
  thing.add_wifi(WIFI_SSID, WIFI_WPA2_PASSWORD);

  // allow reading plant water/humidity
  thing["water"] >> [](pson& out){ out = !digitalRead(SENSOR_PIN); };

  // allow activating and deactivating led alarm remotelly
  thing["led_alarm"]["show"] << [](pson& in){ 
    if(in.is_empty()) in = showLedAlarm;
    else showLedAlarm = in;
  };

  // allow changing led alarm frequency
  thing["led_alarm"]["freq"] << [](pson& in){ 
    if(in.is_empty()) in = ledAlarmInterval;
    else ledAlarmInterval = in;
  }; 

  // allow changing hysteresisTime remotelly
  thing["hysteresis"] << [](pson& in){ 
    if(in.is_empty()) in = hysteresisTime;
    else hysteresisTime = in;
  };
}

void loop() {
  thing.handle();

  // read water value
  bool water = !digitalRead(SENSOR_PIN);
  
  if(water){
    if(waterState!=WATER){
      digitalWrite(LED_PIN, LOW); // turn off led alarm
      waterState = WATER; // reset water state
    }
  }else{
     unsigned long currentMillis = millis();
     switch(waterState){
        case WATER:
            timeWithoutWater = 0;
            previousMillis = currentMillis;
            waterState = START_DETECTING_NO_WATER;
          break;
        case START_DETECTING_NO_WATER:
          timeWithoutWater += (currentMillis - previousMillis);
          previousMillis = currentMillis;
          // only notify after a hysteresis time to prevent drifts in sensor reading
          if(timeWithoutWater>=hysteresisTime){
            waterState = NO_WATER;
            thing.call_endpoint(ALARM_ENDPOINT);
          }
          break;
        case NO_WATER:
          if(currentMillis - previousMillis > ledAlarmInterval) {
            previousMillis = currentMillis;   
            ledState = ledState == LOW ? HIGH & showLedAlarm : LOW;
            digitalWrite(LED_PIN, ledState);
          }
          break;
     }
  }
}