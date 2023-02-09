// Libraries
#include <ESP8266WiFi.h>
#include "ThingsBoard.h"

// Wifi connection setting
#define WIFI_AP "MINH MUP_2.4G"
#define WIFI_PASSWORD "28051989"

// Thingsboard device token (auto generated)
#define TOKEN "LivingroomLight"
bool switchLED = false;
// NODEMCU pinout, use for remote GPIO controlling
#define GPIO1 D1
// #define GPIO2 D2
#define GPIO1_PIN 1
// #define GPIO2_PIN 2
#define SWITCH D2              // BUTTON SWITCH OF LED
// Thingsboard Edge on-premise local IP
char thingsboardServer[] = "192.168.1.12";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

// We assume that all GPIOs are LOW
boolean gpioState = false;

String get_gpio_status() {
    StaticJsonDocument<200> jsonDoc;
    jsonDoc[String(1)] = gpioState ? true : false; // NodeMCU is backwards, default should be HIGH : LOW for arduino
    char payload[256];
    serializeJson(jsonDoc, payload);
    String strPayload = String(payload);
    Serial.print("Get gpio status: ");
    Serial.println(strPayload);
    return strPayload;
}

void set_gpio_status(boolean enabled) {
  // Output GPIOs state
  digitalWrite(GPIO1, enabled ? HIGH : LOW); // NodeMCU is backwards, Arduino is HIGH : LOW
  // Update GPIOs state
  gpioState = enabled;
}

// The callback for when a PUBLISH message is received from the server.
void on_message(const char* topic, byte* payload, unsigned int length) {

  Serial.println("On message");

  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  // Decode JSON request
  StaticJsonDocument<200> jsonDoc;
  auto error = deserializeJson(jsonDoc, (char*)json);
  
  if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
  }

  // Check request method
  String methodName = String((const char*)jsonDoc["method"]);

  // Reply with GPIO status
  if (methodName.equals("getGpioStatus")) {
    
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    
  } 
  
  else if (methodName.equals("setGpioStatus")) {
    
    // Update GPIO status and reply
    set_gpio_status(jsonDoc["params"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }
}

bool oldgpioState = false;

void changeState() {
  if (gpioState != oldgpioState)
    oldgpioState = gpioState;
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
    delay (1000);
}

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
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
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
      // Subscribing to receive RPC requests
      client.subscribe("v1/devices/me/rpc/request/+");
      // Sending current GPIO status
      Serial.println("Sending current GPIO status ...");
      client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
    } 
    else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  Serial.print("Setup");
  
  // Set output mode for all GPIO pins
  pinMode(GPIO1, OUTPUT);
  // pinMode(GPIO2, OUTPUT);
  pinMode(SWITCH, INPUT);

  delay(10);
  
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  client.setCallback(on_message);

}

void loop() {
  switchLED = digitalRead(SWITCH);
  if (switchLED)
    gpioState = true;
  else
    gpioState = false;
  if ( !client.connected() ) {
    reconnect();
  }
  // changeState();
  client.loop();
}