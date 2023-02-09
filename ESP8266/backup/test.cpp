#include <ESP8266WiFi.h>
#include "ThingsBoard.h"   

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// Define AP Name and Password
#define WIFI_AP_NAME "MINH MUP_2.4G"
#define WIFI_PASSWORD "28051989"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/ 
// to understand how to obtain an access token
#define TOKEN "LivingroomLight"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "192.168.1.12"


WiFiClient wifiClient;         // Initialize ThingsBoard client
ThingsBoard tb(wifiClient);    // Initialize ThingsBoard instance
// PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;   // The Wifi radio's status

// Define PIN
uint8 light = D1;              // LED
#define SWITCH D2              // BUTTON SWITCH OF LED
#define LIGHT_SENSOR D3        // LIGHT SENSOR

// Array with LEDs that should be lit up one by one if using muntiple LEDs
// uint8_t leds[] = {D1, D2, D3, D4, D5};
// size_t leds_size = COUNT_OF(leds);

int quant = 20;                // Initial period of LED cycling.
int led_delay = 10000;         // Time passed after LED was turned ON, milliseconds.
int led_passed = 0;            // Variable to store data from server command
bool subscribed = false;       // Set to true if application is subscribed for the RPC messages.
bool LEDState = false;         // Current state of light
bool lightSensorState = false; // actual read value from LIGHT_SENSOR
bool setDelay = false;         // True if there are any RPC command
bool switchLED = false;        // actual read value from SWITCH
bool oldswitchLED = false;     // last read value from SWITCH
bool timerStart = false;       // variable for timing light sensor 
unsigned long lastTime;        // variable for timing light sensor 

// String get_gpio_status() {
//     StaticJsonDocument<200> jsonDoc;
//     jsonDoc[String(1)] = LEDState ? true : false;
//     // jsonDoc[String(light)] = LEDState ? true : false; // NodeMCU is backwards, default should be HIGH : LOW for arduino
//     char payload[256];
//     serializeJson(jsonDoc, payload);
//     String strPayload = String(payload);
//     Serial.print("Get gpio status: ");
//     Serial.println(strPayload);
//     return strPayload;
// }
// Processes function for RPC call "setValue"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processDelayChange(const RPC_Data &data) {
  Serial.println("Received the set delay RPC method");
  // Process data
  led_delay = data;
  Serial.print("Set new delay: ");
  Serial.println(led_delay);
  setDelay = true;
  return RPC_Response(NULL, led_delay);
}
// Processes function for RPC call "getValue"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processGetDelay(const RPC_Data &data) {
  Serial.println("Received the get value method");
  return RPC_Response(NULL, led_delay);
}
// RPC handlers
RPC_Callback callbacks[] = {
  { "setValue",         processDelayChange },
  { "getValue",         processGetDelay },
  // { "setGpioStatus",    processSetGpioState },
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
  switchLED = digitalRead(SWITCH);                // read the pushButton State
  lightSensorState = !digitalRead(LIGHT_SENSOR);  // read the IR Sensor State
  // LED control using button
  if (switchLED != oldswitchLED) {                
    oldswitchLED = switchLED;
    delay(200);
    if (switchLED) {
      LEDState = !LEDState;
      timerStart = false;
      tb.sendAttributeBool("1", LEDState);
      Serial.println("Sent Client Attribute Data");
    }
  }
  // LED control using RPC Response
  if (setDelay) {
    if(led_delay == 1)  
      LEDState = true;                           
    else if (led_delay == 0)  
      LEDState = false;
    setDelay = false;
  }
  // LED auto turned on by sensor
  if (lightSensorState) {  
    LEDState = true;                       
    lastTime = millis();
    timerStart = true;
  }
  // LED auto turned off in 5s if sensor reads LOW logic level
  else if (millis() - lastTime > 5000 && timerStart)
    LEDState = false;
  // Turn on/off LED based on LEDState
  if (LEDState)           
    digitalWrite(light, HIGH);
  else                      
    digitalWrite(light, LOW);
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(112500);
  InitWiFi();
  // Pin config
  pinMode(light, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(LIGHT_SENSOR, INPUT);
  lastTime = 0;
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