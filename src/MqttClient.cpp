#include <String.h>
#include "MqttClient.h"

using namespace SpaIot;

#include <map>


// Convert MqttTopic --> Type	/   Type --> MqttTopic
const std::map<String, Event::Type> MqttClientClass::Type = {
    {"power", Event::Type::PowerOn},
    {"bubble", Event::Type::BubbleOn},
    {"filter", Event::Type::FilterOn},
    {"heater", Event::Type::HeaterOn},
    {"jet", Event::Type::JetOn},
    {"sanitizer", Event::Type::SanitizerOn},
    {"heat_reached", Event::Type::HeatReached},
    {"desired_temp", Event::Type::SetDesiredTemp},
    {"water_temp", Event::Type::WaterTemp},
    {"sanitizer_time", Event::Type::SanitizerTime},
    {"error_code", Event::Type::ErrorCode},
    {"ambient_temperature", Event::Type::AmbientTemperature},
    {"no_event", Event::Type::NoEvent},
    {"any_event", Event::Type::AnyEvent}
};

const std::map<Event::Type, String> MqttClientClass::MqttTopic = {
    {Event::Type::PowerOn, "power"},
    {Event::Type::BubbleOn, "bubble"},
    {Event::Type::FilterOn, "filter"},
    {Event::Type::HeaterOn, "heater"},
    {Event::Type::JetOn, "jet"},
    {Event::Type::SanitizerOn, "sanitizer"},
    {Event::Type::HeatReached, "heat_reached"},
    {Event::Type::SetDesiredTemp, "desired_temp"},
    {Event::Type::WaterTemp, "water_temp"},
    {Event::Type::SanitizerTime, "sanitizer_time"},
    {Event::Type::ErrorCode, "error_code"},
    {Event::Type::AmbientTemperature, "ambient_temperature"},
    {Event::Type::NoEvent, "no_event"},
    {Event::Type::AnyEvent, "any_event"}
};


// Constructor
MqttClientClass::MqttClientClass() : SpaIot::SpaClient("mqttClientClass") {}


// Functions
void MqttClientClass::begin(const MqttSettings &settings, Client &client) {
    // Configure the MQTT Client
    // Store the settings
    // Call reconnect

    m_mqtt.setClient(client);
    m_mqtt.setServer(settings.server.c_str(), settings.port);
    m_mqtt.setCallback(callback);
    reconnect();
}

bool MqttClientClass::isOpen() const {
    return m_mqtt.connected();
}

void MqttClientClass::reconnect() {
    // While not connected, call m_mqtt.connect()
    // If connected, call m_mqtt.subscribe()

    while(!m_mqtt.connected()) {
        Serial.println("Attempting MQTT connection...");
        m_mqtt.connect(m_settings.server.c_str(), m_settings.username.c_str(), m_settings.password.c_str());
        if (m_mqtt.connected()) {
            Serial.println("Connected");
            m_mqtt.subscribe("spa");
        } else {
            Serial.print("Failed, state : ");
            Serial.println(m_mqtt.state());
            Serial.println("Try again in 5 seconds");
            delay(5000);
        }
    
    }
}

void MqttClientClass::callback(char *topic, byte *payload, unsigned int length) {
    String p;
    String t (topic);
    Event e;

    p.concat((char *)payload, length);
    Serial.printf("New message : ", topic, p.c_str());
    e.setType(Type.at(t));
    if (e.isBoolean()) {
        e.setValue(p == "on"); // Set e to 1 if p == "on"
    } else {
        e.setValue(p.toInt()); // Set e to 0 (string conversion without numbers return 0)
    }
    mqttClient.pushToSpa(e);
}

bool MqttClientClass::handle() {
    // If not connected, call reconnect
    // Call a m_mqtt.loop()
    // Process incoming messages from spa, by calling pullFromSpa
    // If message are available, call m_mqtt.publish() and return true, else false

    Event e;
    if (!m_mqtt.connected()) {
        reconnect();
    }

    m_mqtt.loop();

    while (pullFromSpa (e)) {
        String topic;
        String payload;

        topic = m_settings.topic;
        if(MqttTopic.count (e.type()) != 0) {
            topic += "/" + MqttTopic.at(e.type());
            if (e.isBoolean()) {
                payload = e.value() == true ? "on" : "off";
            } else {
                payload = String(e.value());
            }

            if (e.type() == Event::Type::PowerOn && e.value() == true) {
                pushToSpa(Event(Event::Type::SetDesiredTemp, 0));
            }

            m_mqtt.publish(topic.c_str(), payload.c_str());
        }
    }
    return true;
}

MqttClientClass mqttClient;