#include <Arduino.h>
#include <SpaIot.h>
#include <WiFi.h>
#include "MqttClient.h"


// OBJECTS
MqttSettings mqttSettings("address", "user", "password", "topic");
String wifiSsid = "wifi ssid";
String wifiPassword = "wifi password";
const unsigned long baudRate = 115200;

WiFiClient wifiClient;

// FUNCTIONS
void setup() {
  Serial.begin(baudRate);
  WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  mqttClient.reconnect();
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}
