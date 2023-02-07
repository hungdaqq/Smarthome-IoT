#include <ESP8266WiFi.h>
#include "ThingsBoard.h"   

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// Define AP Name and Password
#define WIFI_AP_NAME "MINH MUP_2.4G"
#define WIFI_PASSWORD "28051989"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/ 
// to understand how to obtain an access token
#define TOKEN "Light_Control"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "192.168.1.12"

WiFiClient wifiClient;         // Initialize ThingsBoard client
ThingsBoard tb(wifiClient);    // Initialize ThingsBoard instance
int status = WL_IDLE_STATUS;   // The Wifi radio's status

// Define PIN
#define SWITCH D2              // BUTTON SWITCH
#define LIGHT_SENSOR D3        // LIGHT SENSOR
uint8 light = D1;              // LED

// Array with LEDs that should be lit up one by one if using muntiple LEDs
// uint8_t leds[] = {D1, D2, D3, D4, D5};
// size_t leds_size = COUNT_OF(leds);

int quant = 20;                // Initial period of LED cycling.
int led_delay = 10000;         // Time passed after LED was turned ON, milliseconds.
int led_passed = 0;            // Variable to store data from server command
bool subscribed = false;       // Set to true if application is subscribed for the RPC messages.
bool lightState = false;       // Current state of light
bool lightSensorState = false; // actual read value from LIGHT_SENSOR
bool setDelay = false;         // True if there are any RPC command
bool blink = false;            // True if there are RPC blink command
int switchState = 0;           // actual read value from SWITCH
int oldSwitchState = 0;        // last read value from SWITCH
bool timerStart = false;       // variable for timing light sensor 
unsigned long lastTime;        // variable for timing light sensor 

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
    if ( tb.connect(THINGSBOARD_SERVER, TOKEN) ) {
      Serial.println( "[DONE]" );
      Serial.printf("Thingsboard Server: %s", THINGSBOARD_SERVER);
      Serial.println("");
    } 
    else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds." );
      // Wait 5 seconds before retrying
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
  switchState = digitalRead(SWITCH);    // read the pushButton State
  lightSensorState = digitalRead(LIGHT_SENSOR);
  if (switchState != oldSwitchState) {
    oldSwitchState = switchState;
    delay(300);
    if (switchState == HIGH)  
      lightState = !lightState;
    if (lightState)           
      digitalWrite(light, HIGH);
    else                      
      digitalWrite(light, LOW);
    blink = false;
    timerStart = false;
  }
  else if (setDelay) {
    if(led_delay == 1)        
      digitalWrite(light, HIGH);     
    else if (led_delay == 0)  
      digitalWrite(light, LOW); 
    else
      blink = true;       
    setDelay = false;
    timerStart = false;
  }
  if (blink) {
    if (lightState)
      digitalWrite(light, HIGH);
    else
      digitalWrite(light, LOW);
    lightState = !lightState;
    led_passed = 0;
  }
  if (lightSensorState) {
    digitalWrite(light, HIGH); 
    timerStart = true;
    lastTime = millis();
  }
  if (millis() - lastTime > 5000 && timerStart) { 
    digitalWrite(light, LOW);
  }
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
  delay(quant);
  led_passed += quant;
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