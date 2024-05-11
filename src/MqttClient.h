#pragma once

#include <Arduino.h>
#include <SpaIot.h>
#include <PubSubClient.h>

class MqttSettings {
    public :
        String server;
        String username;
        String password;
        String topic;
        int port;
        MqttSettings(String server, String username, String password, String topic, int port = 1883) :
            server(server), username(username), password(password), topic(topic), port(port) {}
        MqttSettings () : port(1883) {}
        MqttSettings(String server, String topic, int port = 1883) :
            server(server), topic(topic), port(port) {}
};

class MqttClientClass : public SpaIot::SpaClient {
    public:
        MqttClientClass(); // Constructor
        void begin(const MqttSettings & settings, Client & client);
        bool handle();
        void reconnect();
        bool isOpen() const;
    
    private:
        PubSubClient m_settings;
        mutable PubSubClient m_mqtt;
        static void callback(char* topic, byte* payload, unsigned int length);
        static const std::map<SpaIot::Event::Type, String> TypeToString;
};

extern MqttClientClass mqttClient;
