#include <ESP8266WiFi.h>
#include "ThingsBoard.h"   

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define WIFI_AP_NAME "MINH MUP_2.4G"
#define WIFI_PASSWORD "28051989"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/ 
// to understand how to obtain an access token
#define TOKEN "Light_Control"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "192.168.1.13"

// Initialize ThingsBoard client
WiFiClient wifiClient;
// Initialize ThingsBoard instance
ThingsBoard tb(wifiClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

#define SWITCH D2
uint8 light = D1;
// Array with LEDs that should be lit up one by one
// uint8_t leds[] = {D1, D2, D3, D4, D5};
// size_t leds_size = COUNT_OF(leds);

// Main application loop delay
int quant = 20;
// Initial period of LED cycling.
int led_delay = 10000; // max 10k ms, min 25 ms
// Time passed after LED was turned ON, milliseconds.
int led_passed = 0;
// Set to true if application is subscribed for the RPC messages.
bool subscribed = false;
// LED number that is currenlty ON.
int current_led = 0;

bool lightState = false;
int switchState = 0; // actual read value from pin4
int oldSwitchState = 0; // last read value from pin4

// Processes function for RPC call "setValue"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processDelayChange(const RPC_Data &data) {
  Serial.println("Received the set delay RPC method");
  // Process data
  led_delay = data;
  Serial.print("Set new delay: ");
  Serial.println(led_delay);
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
  switchState = digitalRead(SWITCH); // read the pushButton State
  if (switchState != oldSwitchState) {
    oldSwitchState = switchState;
  }
  if (switchState == HIGH){
    lightState = !lightState;
  }
  if (led_delay == 1 || lightState) {
    digitalWrite(light, HIGH);
  }
  else if(led_delay == 0 || !lightState) {
    digitalWrite(light, LOW);
  }    
  else if (led_passed > led_delay) {
    if (lightState) {
      digitalWrite(light, HIGH);
    }
    else {
      digitalWrite(light, LOW);
    }
    lightState = !lightState;
    led_passed = 0;
  }
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(112500);
  InitWiFi();
  // Pinconfig
  pinMode(light, OUTPUT);
  pinMode(SWITCH, INPUT);
}

// Main application loop
void loop() {
  delay(quant);
  led_passed += quant;
  lightControl();
  // Reconnect to ThingsBoard, if needed
  if (!tb.connected()) {
    subscribed = false;
    // Connect to the ThingsBoard
    reconnectTB();
  }
  // Subscribe for RPC, if needed
  if (!subscribed) {
    subcribeRPC();
  }
  // Process messages
  tb.loop();
}