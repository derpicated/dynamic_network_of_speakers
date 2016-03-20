#include "Config.h"
#include "DNSMusic.h"
#include <mosquittopp.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <assert.h>


DNSMusic::DNSMusic(	const std::string& appname,
                   	const std::string& clientname,
                    const std::string& host,
                    int port)
:  	mosqpp::mosquittopp {(CLIENT_XXX + appname + clientname).c_str()}
, 	_appname {appname}
, 	_clientname {clientname}
, 	_topicRoot {"ESEiot"}
,	_dataStore {}
, 	Position {0}
, 	Angle {0}
, 	Volume{0}
, 	Stop {true}
, 	Play {false}
, 	JsonDataString {""}
, 	_cv {}
,	_mtx {}
, 	_running {true}
, 	_thread {&DNSMusic::processData, this}

{
	_topicRoot.add("DNS");

    will_set( MQTT_TOPIC_INFO_CLIENT_OFFLINE.c_str(), CLIENT_XXX.size(),
    			CLIENT_XXX.c_str(), MQTT_QoS_0);

  	connect(host.c_str(), port, MQTT_KEEP_ALIVE); 
}

DNSMusic::~DNSMusic()
{
	std::cerr << "---- ** disconnecting DNSMusic" << std::endl;
	publish(nullptr, MQTT_TOPIC_INFO_CLIENT_OFFLINE.c_str(), CLIENT_XXX.size(), 
  	 			CLIENT_XXX.c_str(), MQTT_QoS_0); 
  	stop();
  	disconnect();
}

void DNSMusic::stop()
{
	_running = false;
	if (_thread.joinable())
	{
		_thread.join();
	}
}


void DNSMusic::on_connect(int rc)
{
  	if (rc == 0)
  	{
  		publish(nullptr, MQTT_TOPIC_INFO_CLIENT_ONLINE.c_str(), CLIENT_XXX.size(), 
  					CLIENT_XXX.c_str(), MQTT_QoS_0);

  		subscribe(nullptr, MQTT_TOPIC_REQUEST_ONLINE.c_str(), MQTT_QoS_0);
  		subscribe(nullptr, MQTT_TOPIC_CLIENTID_OBJECTID.c_str(), MQTT_QoS_0);
  		subscribe(nullptr, MQTT_TOPIC_INFO_MUSIC_VOLUME.c_str(), MQTT_QoS_0);
  		subscribe(nullptr, MQTT_TOPIC_INFO_MUSIC_PS.c_str(), MQTT_QoS_0);
  	}
}


void DNSMusic::on_disconnect(int rc)
{
	if(!(rc == 0))
	{
		publish(nullptr, MQTT_TOPIC_INFO_CLIENT_OFFLINE.c_str(), CLIENT_XXX.size(), 
	  	 		CLIENT_XXX.c_str(), MQTT_QoS_0);
	}
	std::cerr << "---- DNSMusic disconnected with rc = " << rc << std::endl;
}


void DNSMusic::on_message(const mosquitto_message *message)
{
	std::unique_lock<std::mutex> lk {_mtx};
	std::string topic {message->topic};

	std::cerr 	<< "DATA received by a topic + message: " << topic << " "
            	<< (char*)message->payload << std::endl;

	std::cerr 	<< "Value of the struct mosquitto_message" 	<< std::endl
				<< "Mid: " 		<< message->mid 			<< std::endl
				<< "topic: " 	<< message->topic 			<< std::endl
				<< "payload: " 	<< (char*)message->payload 
	 			<< " lengte: " 	<< message->payloadlen 		<< std::endl
	 			<< "QOS: " 		<< message->qos 			<< std::endl
	 			<< "Retian" 	<< message->retain 			<< std::endl << std::endl;

	 if(topic.compare(MQTT_TOPIC_REQUEST_ONLINE) == 0){
	 		publish(nullptr, MQTT_TOPIC_INFO_CLIENT_ONLINE.c_str(), CLIENT_XXX.size(), 
	  	 		CLIENT_XXX.c_str(), MQTT_QoS_0);
	}

	if (topic.compare(MQTT_TOPIC_INFO_MUSIC_VOLUME) == 0){
		try{
				size_t size {0};
				std::string sVolume = {(char*)message->payload};
				Volume = stoi(sVolume, &size);
				if(sVolume.size() != size)
				{
					std::cerr << "wrong volume value" << std::endl;
				}
		}
		catch(std::invalid_argument& e)
		{
		  std::cerr << std::endl << "## EXCEPTION s40 invalid_argument: " << e.what() << std::endl;
		}
		catch(std::out_of_range& e)
		{
		  std::cerr << std::endl << "## EXCEPTION s40 out_of_range: " << e.what() << std::endl;
		}
	}

	if (topic.compare(MQTT_TOPIC_INFO_MUSIC_PS) == 0){
			std::string PS = {(char*)message->payload};
			if (!PS.compare("p") || !PS.compare("play"))
			{
				Play = true;
				Stop = false;
			}
			if (!PS.compare("s") || !PS.compare("stop"))
			{
				Play = false;
				Stop = true;
			}
	}

	if (topic.compare(MQTT_TOPIC_CLIENTID_OBJECTID) == 0){
		JsonDataString = (char*)message->payload;  		
	}
}

void DNSMusic::on_subscribe(int mid, int qos_count,
                           	const int *granted_qos)
{
	std::cerr	<< "---- DNSMusic subscription succeeded mid = " << mid << std::endl
 		    << " qos_count = " << qos_count	<<	std::endl 
       		<< " granted_qos = " << *granted_qos << std::endl;
}


void DNSMusic::on_log(int level, const char *str)
{
	std::cerr	<< "---- # log DNSMusic " << level << std::endl
	 	   		<< ": " << str << std::endl;
}

void  DNSMusic::on_error()
{
	std::cerr << "**** DNSMusic ERROR" << std::endl;
}



void DNSMusic::processData()
{
	std::chrono::milliseconds Tms {5000};
	while(_running)
	{

		std::cerr << "this is a thread     " <<  "Client name: " << CLIENT_XXX << std::endl<< std::endl;
		std::cerr << "volume = " << std::setprecision(3)<< Volume << std::endl;
		std::cerr << "Play " << Play << "  " << "Stop " << Stop << std::endl;
		std::cerr << "JsonDataString" << JsonDataString << std::endl; 

		std::this_thread::sleep_for(Tms);
	}
}







