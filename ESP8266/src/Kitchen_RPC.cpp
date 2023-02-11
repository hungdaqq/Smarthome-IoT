#include <ESP8266WiFi.h>
#include "ThingsBoard.h"   

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// Define AP Name and Password
// #define WIFI_AP_NAME "MINH MUP_2.4G"
// #define WIFI_PASSWORD "28051989"
#define WIFI_AP_NAME "4H"
#define WIFI_PASSWORD "88998899"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/ 
// to understand how to obtain an access token
#define TOKEN "KitchenLight"
// ThingsBoard server instance
// Replace your TB Egde IP address
#define THINGSBOARD_SERVER  "192.168.1.7" 


WiFiClient wifiClient;         // Initialize ThingsBoard client
ThingsBoard tb(wifiClient);    // Initialize ThingsBoard instance
// PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;   // The Wifi radio's status

// Define PIN
uint8 light = D1;              // LED
#define SENSOR D2              // BUTTON SWITCH OF LED
#define BUZZER D3              // BUZZER
#define LEDSWITCH D5           // LED SWITCH
#define BUZZERSWITCH D6        // BUZZER SWITCH

bool subscribed = false;       // Set to true if application is subscribed for the RPC messages
bool LEDState = false;         // Current state of light
bool switchLED = false;        // actual read value from SWITCH
bool oldswitchLED = false;     // last read value from SWITCH
bool buzzerState = false;      // Current state of buzzers
bool switchBuzzer = false;     // actual read value from SWITCH
bool oldswitchBuzzer = false;  // last read value from SWITCH
bool sensorState = false;
// unsigned long lastTime;     // variable for timing fire sensor 

// Processes function for RPC call "setValue"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processChangeState(const RPC_Data &data) {
  Serial.println("Received the set state RPC method");
  // Process data
  LEDState = data;
  Serial.print("Set new LED state: ");
  Serial.println(LEDState);
  return RPC_Response(NULL, LEDState);
}
// Processes function for RPC call "getValue"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processGetState(const RPC_Data &data) {
  Serial.println("Received the get value method");
  return RPC_Response(NULL, LEDState);
}
// RPC handlers
RPC_Callback callbacks[] = {
  { "setState",   processChangeState  },
  { "getState",   processGetState     },
};

void reconnectWiFi() {
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: "); 
  // Show IP of this device
  Serial.println(WiFi.localIP());
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
    // attempt to connect to TB Sever with provided TOKEN
    if ( tb.connect(THINGSBOARD_SERVER, TOKEN) ) {
      Serial.println( "[DONE]" );
      Serial.printf("Thingsboard Server: %s", THINGSBOARD_SERVER);
      Serial.println("");
    } 
    else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds." );
      // Wait 5 seconds before retrying if FAILED
      delay( 5000 );
    }
  }
}

void subcribeRPC(){
  Serial.print("Subscribing for RPC ... ");
  // Perform a subscription. All consequent data processing will happen in
  // callbacks as denoted by callbacks[] array.
  if (tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
    Serial.println( "[DONE]" );
    subscribed = true;
  }
  else {
    Serial.println( "[FAILED]" );
    Serial.println( " : retrying in 3 seconds." );
    // Wait 5 seconds before retrying
    delay( 3000 );
  }
}
void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  reconnectWiFi();
}
void lightControl() {
  switchLED = digitalRead(LEDSWITCH);       // read the pushButton LED State
  switchBuzzer = digitalRead(BUZZERSWITCH); // read the pushButton Buzzer State
  sensorState = !digitalRead(SENSOR);       // read the IR Sensor State
  // LED control using button
  if (switchLED != oldswitchLED) { 
    // Compare current state with last state               
    oldswitchLED = switchLED;
    delay(200);
    if (switchLED) {
      LEDState = !LEDState;
      // Send client attribute
      tb.sendAttributeBool("1", LEDState);
      Serial.println("Sent Client Attribute LED data");
    }
  }
  if (switchBuzzer != oldswitchBuzzer) { 
    // Compare current state with last state               
    oldswitchBuzzer = switchBuzzer;
    delay(200);
    if (switchBuzzer) {
      buzzerState = !buzzerState;
      tb.sendAttributeBool("buzzer", buzzerState);
      Serial.println("Sent Client Attribute Buzzer data");
    }
  }
  // Buzzer and LED are auto turned on by sensor
  if (sensorState) {  
    LEDState = true;
    buzzerState = true;                      
    // lastTime = millis();
    // digitalWrite(BUZZER, HIGH);
    // Send client attribute
    tb.sendAttributeBool("buzzer", buzzerState);
    Serial.println("Sent Client Attribute Buzzer data");
  }
  // BUZZER auto turned off in 5s if sensor doesn't get any fire
  // else if (millis() - lastTime > 500)
  //   digitalWrite(BUZZER, LOW);
  // Turn on/off LED based on LEDState
  if (LEDState)           
    digitalWrite(light, HIGH);
  else                      
    digitalWrite(light, LOW);
  // Turn on/off Buzzer based on buzzerState  
  if (buzzerState)           
    digitalWrite(BUZZER, HIGH);
  else                      
    digitalWrite(BUZZER, LOW);
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(112500);
  InitWiFi();
  // Pin config
  pinMode(light, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LEDSWITCH, INPUT);
  pinMode(BUZZERSWITCH, INPUT);
  pinMode(SENSOR, INPUT);
  // lastTime = 0;
}

// Main application loop
void loop() {
  lightControl();
  if (!tb.connected()) {    // Reconnect to ThingsBoard, if needed
    subscribed = false;     // Connect to the ThingsBoard
    reconnectTB();
  }
  if (!subscribed) {        // Subscribe for RPC, if needed
    subcribeRPC();
  }
  tb.loop();                // Process messages
}