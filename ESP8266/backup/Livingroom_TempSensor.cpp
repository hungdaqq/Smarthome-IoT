#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}
#include <ThingsBoard.h>
#include <TM1637Display.h>

#define WIFI_AP "MINH MUP_2.4G"
#define WIFI_PASSWORD "28051989"

// This device access token
#define TOKEN "LivingroomTemperatureSensor"

// DS18B20 sensor pinout
OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);

// LED module pinout: TM1637Display display(CLK,DIO);
TM1637Display display(D1,D2);

// Temperature threshold
#define RED D5
#define YELLOW D6
#define GREEN D7
const int red = 60;
const int yellow = 40;
const int green = 20;

// unsigned long lastSend;

// ThingsBoard server instance.
// Use "demo.thingsboard.io" to send data directly to Live Demo server
// Use local IP Address of TB Edge to send data to Edge database
char thingsboardServer[] = "192.168.1.12";
// Initialize ThingsBoard client
WiFiClient wifiClient;
// Initialize ThingsBoard instance
ThingsBoard tb(wifiClient);
 
int status = WL_IDLE_STATUS;

void setState(char STATE){
  // LED State: 'g' -> green, 'y' -> yellow, 'r' -> red, 'a' -> all led, 'n' -> none led
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
  else if (STATE == 'n') {
    digitalWrite(RED, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);       
  }
  else if (STATE == 'a') {
    digitalWrite(RED, HIGH);
    digitalWrite(YELLOW, HIGH);
    digitalWrite(GREEN, HIGH);       
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
    setState('a');
  }
  else if (temperature <= green)
    setState('g');
  else if (temperature <= yellow && temperature > green)
    setState('y');
  else
    setState('r');
  // Display on LED module
  display.showNumberDec(round(temperature*10));
  // Print temperature on Serial port
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C.");
  // Send TB
  tb.sendTelemetryFloat("temperature", temperature);
}

void reconnectWiFi() {
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    // RED LED blinks when WiFi disconnected
    delay(500);
    setState('r');
    Serial.print(".");
    delay(500);
    setState('n');
  }
  // GREEN LED blinks 3 times when WiFi connected
  for (int i = 0; i < 3; i++){
    delay(500);
    setState('g');
    delay(500);
    setState('n');
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: "); 
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
      Serial.println( "[DONE]" );
      Serial.printf("Thingsboard Server: %s", thingsboardServer);
      Serial.println("");
    } 
    else {
      // Failed
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds." );
      // Wait 5 seconds before retrying
      for (int i = 0; i < 5; i++){
        // RED LED blinks in 5 senconds
        delay(500);
        setState('r');
        delay(500);
        setState('n');
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Pinout LED to control
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  // Adjusting the brightness of 4 Digit 7 segment display module. Range: 0 to 7
  display.setBrightness(4);
  // Start the temperature sensor
  sensors.begin();
  delay(10);
  // Initialize WiFi connection
  InitWiFi();
  // Setting auto-sleep type: LIGHT_SLEEP_T (or MODEM_SLEEP_T)
  wifi_set_sleep_type(LIGHT_SLEEP_T);
  // In Light sleep mode CPU: Pending, WiFi: OFF, current: 0.4 mA
  // In Modem sleep mode CPU: ON, WiFi: OFF, current: 15 mA
}

void loop() {
  // Try to reconnect to Thingsboard Sever
  if ( !tb.connected() ) {
    reconnectTB();
  }
  getAndSendTemperatureAndHumidityData();
  // This is script to force MODEM_SLEEP mode, but i don't recommend to use it. 
  // WiFi.disconnect();
  // WiFi.forceSleepBegin();
  // Auto Light sleep will be enabled if the WiFi and CPU is free for >10 senconds (tested).
  delay(5e3);
  tb.loop();
}