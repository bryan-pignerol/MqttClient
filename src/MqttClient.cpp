#include <String.h>
#include "MqttClient.h"

using namespace SpaIot;

#include <map>

std::map<Event::Type, String> TypeToString = {
    {Event::Type::PowerOn, "power"},
    {Event::Type::BubbleOn, "bubble"},
};

MqttClientClass::MqttClientClass() : SpaIot::SpaClient("mqttClientClass") {
    TypeToString[Event::PowerOn] = "power";
}

void MqttClientClass::begin(const MqttSettings & settings, Client & client) {
    // Configure the MQTT Client
    // Store the settings
    // Call reconnect

    m_mqtt.setClient(client);
    m_mqtt.setServer(settings.server.c_str(), settings.port);
    m_mqtt.setCallback(callback);
    reconnect();
}

bool MqttClientClass::handle() {
    // If not connected, call reconnect
    // Call a m_mqtt.loop()
    // Process incoming messages from spa, by calling pullFromSpa
    // If message are available, call m_mqtt.publish() and return true, else false

    if (!m_mqtt.connected()) {
        reconnect();
    }
    m_mqtt.loop();
    if (pullFromSpa) {
        m_mqtt.publish("spa", "message");
        return true;
    } else {
        return false;
    }
}

void MqttClientClass::reconnect() {
    // While not connected, call m_mqtt.connect()
    // If connected, call m_mqtt.subscribe()

    while(!m_mqtt.connected()) {
        Serial.println("Attempting MQTT connection...");
        m_mqtt.connect("jeedom.btssn.lan", "jeedom", "W+pL(69g8u8$hY");
        if (m_mqtt.connected()) {
            Serial.println("Connected");
            m_mqtt.subscribe("bryanspa");
        } else {
            Serial.print("Failed, state : ");
            Serial.println(m_mqtt.state());
            Serial.println("Try again in 5 seconds");
            delay(5000);
        }
    
    }
}

bool MqttClientClass::isOpen() const {
    return m_mqtt.connected();
}

void MqttClientClass::callback(char * topic, byte * payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
}
