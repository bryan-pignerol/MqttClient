#pragma once

#include <Arduino.h>
#include <SpaIot.h>
#include <PubSubClient.h>


// On crée une classe stockant les paramètres du broker MQTT
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


// On crée une classe MqttClientClass héritant de SpaIot::SpaClient
class MqttClientClass : public SpaIot::SpaClient {
    public:
        MqttClientClass(); // Constructeur
        // On définit ici les différentes fonctions de la classe
        void begin(const MqttSettings & settings, Client & client);
        bool handle();
        void reconnect();
        bool isOpen() const;
    
    private:
        MqttSettings m_settings; // On stocke les paramètres du broker MQTT
        mutable PubSubClient m_mqtt; // On crée un objet PubSubClient
        static void callback(char* topic, byte* payload, unsigned int length); // Fonction callback
        static const std::map<String, SpaIot::Event::Type> Type; // Conversion MqttCommand --> Type
        static const std::map<SpaIot::Event::Type, String> MqttTopic; // Conversion Type --> MqttTopic
};

extern MqttClientClass mqttClient; // On crée un objet MqttClientClass
