#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "libs/jzon/Jzon.h"

const int NUMBEROFBROKERS {4};
const std::string CONFIGPATH_IN {"config/config.js"};
const std::string CONFIGPATH_OUT {"config/config.json"};



struct Broker
{
	std::string name = "Mosquitto" ;
	std::string uri  = "test.mosquitto.org";
	int port = 1883 ;
	int port_encrypted = 8883;
	int port_encrypted_certificate = 8884;
	int port_ws = 8080;
	int port_ws_encrypted = 8081;
};


class ConfigFileReader
{
public:
	ConfigFileReader();
	~ConfigFileReader();

	void getDataFromFile();
	void writeDataToFile();
	void parseData();
	std::string getProjectName(bool fullname = true);
	std::string getVersion();
	std::string getSpeakerName();
	std::string getWebsiteName();
	Broker getBroker(int broker = 1);

	void printJsonData();
	void printBroker(int broker);


protected:

private:
	std::string FileData;
	std::string JsonData;
	std::string ProjectName;
	std::string ShortProjectName;
	int VersionMajor, VersionMinor, VersionRevision;
	std::string SpeakerName;
	std::string WebsiteName;
	int BrokerToUse;

	Jzon::Parser _filereader;
	Broker brokers[NUMBEROFBROKERS];
};



#endif
