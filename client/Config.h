#ifndef CONFIG_H
#define CONFIG_H

#include <string>

const char* getBBBid();
const char* getClientID();

const std::string BBB_ID {getBBBid()};
const std::string CLIENT_XXX {getClientID()};

const int MQTT_KEEP_ALIVE {60}; 
const int MQTT_QoS_0 {0};
const int MQTT_QoS_1 {1};
const int MQTT_QoS_2 {2};
const bool MQTT_RETAIN_OFF {false};
const bool MQTT_RETAIN_ON {true};

const std::string MQTT_BROKER {"iot.eclipse.org"};//default: test.mosquitto.org
const int MQTT_BROKER_PORT = 1883;//default: 1883

const std::string MUSIC_LOCATION {"/home/brian/Mumford_&_Sons-The_Wolf_(Live).mp3"};

/*this one is also declared in the construtor of DNSMusic class*/
const std::string MQTT_TOPIC_ROOT 					{"ESEiot/DNS"};

const std::string MQTT_TOPIC_REQUEST_ONLINE 		{"ESEiot/DNS/request/online"};
const std::string MQTT_TOPIC_REQUEST_RWF			{"ESEiot/DNS/request/update/rwf"};

const std::string MQTT_TOPIC_INFO_CLIENT_ONLINE 	{"ESEiot/DNS/info/client/online"};
const std::string MQTT_TOPIC_INFO_CLIENT_OFFLINE 	{"ESEiot/DNS/info/client/offline"};
const std::string MQTT_TOPIC_INFO_CLIENT_LASTWILL 	{"ESEiot/DNS/info/client/lastwill"};
const std::string MQTT_TOPIC_INFO_MUSIC_VOLUME 		{"ESEiot/DNS/info/music/volume"};
const std::string MQTT_TOPIC_INFO_MUSIC_PS 			{"ESEiot/DNS/info/music/status"};


const std::string MQTT_TOPIC_CLIENTID_OBJECTID 		{"ESEiot/DNS/client/speakers/"+ CLIENT_XXX +"/objects"};

// /*json data */
// {
//   "clientid": { "objectid":  	{"distance": 0, "angle": 0 },
//     			"objectid2": 	{"distance": 0,	"angle": 0 }
//     			}
// }





#endif
