#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}
#include <ThingsBoard.h>
#include <Led4digit74HC595.h>
#include "ShiftDisplay.h"

#define WIFI_AP "MINH MUP_2.4G"
#define WIFI_PASSWORD "28051989"

#define TOKEN "Kitchen_Temperature_Sensor"

// DS18B20
OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);

// LED display
// Led4digit74HC595 display(D1,D2,D3);
ShiftDisplay display(D2,D1,D3,COMMON_CATHODE,4);

// Threshold
#define RED D5
#define YELLOW D6
#define GREEN D7
const int red = 60;
const int yellow = 40;
const int green = 20;

unsigned long lastSend;

char thingsboardServer[] = "192.168.1.13";

WiFiClient wifiClient;
ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;

void setState(char STATE){
  if (STATE == 'g') {
    digitalWrite(GREEN, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);
  }
  else if (STATE == 'y') {
    digitalWrite(YELLOW, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
  }
  else if (STATE == 'r') {
    digitalWrite(RED, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);    
  }
}

void getAndSendTemperatureAndHumidityData() {
  Serial.println("Collecting temperature data.");

  // Read temperature as Celsius (the default)
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature)) {
    Serial.println("Failed to read from sensor!");
    setState('r');
  }
  else if (temperature <= green){
    setState('g');
  }
  else if (temperature <= yellow && temperature > green){
    setState('y');
  }
  else {
    setState('r');
  }
  // Display
  display.set(temperature);
  // display.set(round(temperature*10));

  // Serial print
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C.");
  // Send TB
  tb.sendTelemetryFloat("temperature", temperature);
}

void InitWiFi() {
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  // display.setDecimalPoint(2); 
  sensors.begin();
  delay(10);
  InitWiFi();
  lastSend = 0;
  wifi_set_sleep_type(LIGHT_SLEEP_T);
}


void loop() {
  if ( !tb.connected() ) {
    reconnect();
  }
  getAndSendTemperatureAndHumidityData();
  display.update();
  delay(5000);
}