#include <Arduino.h>
#include <SpaIot.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "MqttClient.h"

using namespace SpaIot;

// OBJECTS
MqttSettings mqttSettings("address broker", "user", "password", "topic");

const char* ssid = "SPA-IOT-T1";
const char* password = "spa_iot_t1";
String wifiSsid = "wifi_ssid";
String wifiPassword = "wifi_password";

String mqttBroker = "broker";
int mqttPort = 1883;
String mqttUser = "user";
String mqttPassword = "password";

const unsigned long baudRate = 115200;

WiFiClient wifiClient;
AsyncWebServer server(80);
SpaServer spaServer;

ControlPanel spa ("SPAIOT32SSP");

// FUNCTIONS
void setup() {
  // On définit l'ESP32 en point d'accès
  Serial.begin(baudRate);
  WiFi.mode(WIFI_AP);
  
  WiFi.softAP(ssid, password);

  // L'ESP32 génère une page web dans laquelle l'utilisateur entrera les informations de son réseau wifi --> 192.168.4.1:80 (URL du site)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String s ="<html><body>";
    s += "<form action=\"/wifi\" method=\"post\">";
    s += "SSID:<br><input type=\"text\" name=\"ssid\"><br>";
    s += "Password:<br><input type=\"text\" name=\"password\"><br>";
    s += "MQTT Server:<br><input type=\"text\" name=\"mqtt_server\"><br>";
    s += "MQTT Port:<br><input type=\"text\" name=\"mqtt_port\"><br>";
    s += "MQTT Username:<br><input type=\"text\" name=\"mqtt_username\"><br>";
    s += "MQTT Password:<br><input type=\"text\" name=\"mqtt_password\"><br>";
    s += "<input type=\"submit\" value=\"Connect\">";
    s += "</form></body></html>";
    request->send(200, "text/html", s);
  });

  // Code effectuée quand l'on appuie sur le bouton en bas de la page web
  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        if(p->name() == "ssid"){
          // Connexion au réseau wifi
          WiFi.begin(p->value().c_str(), request->getParam("password", true)->value().c_str());
          wifiSsid = p->value().c_str();
          wifiPassword = request->getParam("password", true)->value().c_str();
          mqttBroker = request->getParam("mqtt_server", true)->value().c_str();
          mqttPort = request->getParam("mqtt_port", true)->value().toInt();
          mqttUser = request->getParam("mqtt_username", true)->value().c_str();
          mqttPassword = request->getParam("mqtt_password", true)->value().c_str();
          mqttSettings = MqttSettings(mqttBroker, mqttUser, mqttPassword, "spa");
          Serial.println(wifiSsid);
          Serial.println(wifiPassword);
          mqttClient.reconnect();
        }
      }
    }
    request->send(200, "text/plain", "Connected");
  });

  Serial.println("Connected to the WiFi network");

  // Connexion au Broker MQTT
  while(!spa.isOpen()) {
    delay(1000);
    Serial.println("Waiting for spa to open...");
  }

  Serial.println("Spa is open");
  
  mqttClient.begin(mqttSettings, wifiClient);
  spaServer.addClient(mqttClient);
}

void loop() {
  spaServer.handle();
}
