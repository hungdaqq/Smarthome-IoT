#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}
#include "ThingsBoard.h"
#include <Led4digit74HC595.h>

// #define WIFI_AP_NAME "MINH MUP_2.4G"
// #define WIFI_PASSWORD "28051989"
#define WIFI_AP_NAME "4H"
#define WIFI_PASSWORD "88998899"

// This device access token
#define TOKEN "BathroomTemperatureSensor"
// ThingsBoard server instance.
// Use "demo.thingsboard.io" to send data directly to Live Demo server
// Use local IP Address of TB Edge to send data to Edge database
char thingsboardServer[] = "192.168.1.7";

// DS18B20 sensor data pin
OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);

// LED module pinout: TM1637Display display(CLK,DIO);
Led4digit74HC595 display(D1,D2,D3);
// variable for timing and setting sensor period
unsigned long timingLive;
unsigned long sensorPeriod;
// Initialize ThingsBoard client
WiFiClient wifiClient;
// Initialize ThingsBoard instance
ThingsBoard tb(wifiClient);
 
int status = WL_IDLE_STATUS;

void getAndSendTemperatureAndHumidityData() {
  Serial.println("Collecting temperature data.");
  // Read temperature as Celsius (the default)
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature)) {
    Serial.println("Failed to read from sensor!");
  }
  // Display on LED module
  display.setNumber(round(temperature*10));
  // Print temperature on Serial port
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C.");
  // Send TB
  // Serial.printf("Get temperature: %d ms", millis()-timing); 
  // Serial.println("");
  tb.sendTelemetryFloat("temperature", temperature);
  // Serial.printf("Send temperature: %d ms", millis()-timing); 
  // Serial.println("");
}

void reconnectWiFi() {
  // timing = millis();
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  // Serial.printf("WiFi connected: %ld ms, IP address: ", millis()-timing); 
  // Show IP of this device
  Serial.println(WiFi.localIP());
}

void InitWiFi() {
  // Initialize WiFi mode: STA-Station as a prerequisite for automatically activating sleep mode.
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to AP ...");
  // Attempt to connect to WiFi network
  reconnectWiFi();
}

void reconnectTB() {
  // timing = millis();
  // Loop until we're reconnected
  while (!tb.connected()) {
    // WiFi.forceSleepWake();
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      reconnectWiFi();
      Serial.println("Connected to AP");
    }
    // Attempt to connect to TB Server with TOKEN provided
    Serial.print("Connecting to ThingsBoard node ... ");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      // Success
      // Serial.printf( "[DONE]: %ld ms", millis()-timing );
      Serial.println("");
      Serial.printf("Thingsboard Server: %s", thingsboardServer);
      Serial.println("");
      
    }
    else {
      // Failed
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds." );
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Adjusting the brightness of 4 Digit 7 segment display module. Range: 0 to 7
  display.setDecimalPoint(2);
  // Start the temperature sensor
  sensors.begin();
  delay(10);
  // Initialize WiFi connection
  InitWiFi();
  // Setting auto-sleep type: LIGHT_SLEEP_T (or MODEM_SLEEP_T)
  // In Light sleep mode CPU: Pending, WiFi: OFF, current: 0.4 mA
  // In Modem sleep mode CPU: ON, WiFi: OFF, current: 15 mA
  wifi_set_sleep_type(MODEM_SLEEP_T);
  timingLive = 0;
  // sensor cycle;
  sensorPeriod = 100*1000; // 100 secs
}


void loop() {
  display.loopShow();
  // Try to reconnect to Thingsboard Sever
  if ( !tb.connected() ) {
    reconnectTB();
  }
  if (timingLive == 0 || millis() - timingLive > sensorPeriod) {
    getAndSendTemperatureAndHumidityData();
    timingLive = millis();
  }
  // This is script to force MODEM_SLEEP mode, but i don't recommend to use it. 
  // WiFi.disconnect();
  // WiFi.forceSleepBegin();
  // Auto Light sleep will be enabled if the WiFi and CPU is free for >10 senconds (tested).
  // Serial.printf("System awake for: %ld ms", millis()-timingLive);
  // Serial.println();
  // delay(sensorPeriod);
  tb.loop();
}