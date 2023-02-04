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

#define TOKEN "Kitchen_Temperature_Sensor"

// DS18B20
OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);

// LED display
TM1637Display display(D1,D2);
// ShiftDisplay display(D2,D1,D3,COMMON_CATHODE,4);

// Threshold
#define RED D5
#define YELLOW D6
#define GREEN D7
const int red = 60;
const int yellow = 40;
const int green = 20;

// unsigned long lastSend;

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
  else if (STATE == 'n') {
    digitalWrite(RED, LOW);
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
  display.showNumberDec(round(temperature*10));

  // Serial print
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C.");
  // Send TB
  tb.sendTelemetryFloat("temperature", temperature);
}

void reconnectWiFi() {
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    setState('r');
    Serial.print(".");
    delay(500);
    setState('n');
  }
  for (int i = 0; i < 3; i++){
    delay(500);
    setState('g');
    delay(500);
    setState('n');
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
}

void InitWiFi() {
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
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
    Serial.print("Connecting to ThingsBoard node ... ");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
      Serial.printf("Thingsboard Server: %s", thingsboardServer);
      Serial.println("");
    } 
    else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      for (int i = 0; i < 5; i++){
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

  // Setting LED 7 Digit display module's brightness. Adjustment range from 0 to 7
  display.setBrightness(4);

  // Start the temperature sensor
  sensors.begin();
  delay(10);

  // Initialize WiFi connection
  InitWiFi();
  /* Setting auto-sleep mode: MODEM_SLEEP_T. In Modem-sleep mode, ESP8266 will close the
  Wi-Fi module circuit between the two DTIM Beacon intervals in order to save power. 
  ESP8266 will be automatically woken up before the next Beacon arrival. 
  The sleep time is decided by the DTIM Beacon interval time of the router. 
  During sleep, ESP8266 can stay connected to the Wi-Fi and receive the interactive
  information from a mobile phone or server. */
  wifi_set_sleep_type(MODEM_SLEEP_T);
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

  // Auto-sleep will be enabled if the WiFi is free for >10 senconds.
  delay(200e3);
  tb.loop();
}