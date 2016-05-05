#ifndef CONFIG_H
#define CONFIG_H

#include <string>

const char* getClientID ();

const int MQTT_KEEP_ALIVE{ 60 };
const int MQTT_QoS_0{ 0 };
const int MQTT_QoS_1{ 1 };
const int MQTT_QoS_2{ 2 };
const bool MQTT_RETAIN_OFF{ false };
const bool MQTT_RETAIN_ON{ true };

const std::string MQTT_BROKER{
    "test.mosquitto.org"
};                                 // default: test.mosquitto.org
const int MQTT_BROKER_PORT = 1883; // default: 1883

const std::string MQTT_TOPIC__{ "ESEiot" };
const std::string MQTT_TOPIC_ROOT{ MQTT_TOPIC__ + "/DNS" };
const std::string MQTT_TOPIC_REQUEST{ MQTT_TOPIC_ROOT + "/request" };
const std::string MQTT_TOPIC_REQUEST_ONLINE{ MQTT_TOPIC_REQUEST + "/online" };
const std::string MQTT_TOPIC_REQUEST_INFO{ MQTT_TOPIC_REQUEST + "/info" };
const std::string MQTT_TOPIC_REQUEST_INFO_CLIENTS{ MQTT_TOPIC_REQUEST_INFO +
    "/clients" };
const std::string MQTT_TOPIC_INFO{ MQTT_TOPIC_ROOT + "/info" };
const std::string MQTT_TOPIC_INFO_CLIENTS{ MQTT_TOPIC_INFO + "/clients" };
const std::string MQTT_TOPIC_INFO_CLIENTS_WILDCARD{ MQTT_TOPIC_INFO_CLIENTS + "/+" };
const std::string MQTT_TOPIC_INFO_CLIENTS_ONLINE{ MQTT_TOPIC_INFO_CLIENTS +
    "/online" };
const std::string MQTT_TOPIC_INFO_CLIENTS_OFFLINE{ MQTT_TOPIC_INFO_CLIENTS +
    "/offline" };
const std::string MQTT_TOPIC_INFO_MUSIC{ MQTT_TOPIC_INFO + "/music" };
const std::string MQTT_TOPIC_INFO_MUSIC_TIME{ MQTT_TOPIC_INFO_MUSIC + "/time" };
const std::string MQTT_TOPIC_INFO_MUSIC_TIME_POSITION{ MQTT_TOPIC_INFO_MUSIC_TIME + "/potition" };
const std::string MQTT_TOPIC_INFO_MUSIC_STATUS{ MQTT_TOPIC_INFO_MUSIC +
    "/status" };
const std::string MQTT_TOPIC_INFO_MUSIC_SOURCES{ MQTT_TOPIC_INFO_MUSIC +
    "/sources" };
const std::string MQTT_TOPIC_INFO_MUSIC_VOLUME{ MQTT_TOPIC_INFO_MUSIC +
    "/volume" };
#endif
