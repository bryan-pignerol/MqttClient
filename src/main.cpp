#include <Arduino.h>
#include <SpaIot.h>
#include <WiFi.h>
#include "MqttClient.h"

using namespace SpaIot;

// OBJECTS
MqttSettings mqttSettings("address broker", "user", "password", "topic");
String wifiSsid = "wifi_ssid";
String wifiPassword = "wifi_password";
const unsigned long baudRate = 115200;

WiFiClient wifiClient;
SpaServer spaServer;

ControlPanel spa ("SPAIOT32SSP");

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

  spaServer.addClient(mqttClient);

  mqttClient.begin(mqttSettings, wifiClient);

  while(!spa.isOpen()) {
    delay(1000);
    Serial.println("Waiting for spa to open...");
  }

  Serial.println("Spa is open");
}

void loop() {
  spaServer.handle();
}
