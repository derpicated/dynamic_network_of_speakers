#include "ConfigFileReader.hpp"


ConfigFileReader::ConfigFileReader():
	FileData{}
, 	JsonData{}
,	ProjectName{}
,	ShortProjectName{}
,	VersionMajor{}
, 	VersionMinor{}
, 	VersionRevision{}
, 	SpeakerName{}
, 	WebsiteName{}
, 	BrokerToUse {}
, 	_filereader ()
{

	getDataFromFile();
	parseData();
}

ConfigFileReader::~ConfigFileReader()
{}


void ConfigFileReader::getDataFromFile()
{
	int counter = 0;
	std::string line;

	std::ifstream configfile (CONFIGPATH_IN, std::fstream::in);

	if (configfile.is_open())
	{
		std::cerr << "----------------------configfile is open" << std::endl;
		
		while(getline(configfile,line))
		{
			FileData.append(line);
			if (counter != 0)
			{
				JsonData.append(line);
				JsonData.append("\n");
			}
			++counter;
		}
		//close file
		configfile.close();

	}
	else{
		std::cerr << "---------------------configfile isn't open!!!!" << std::endl;
	}

	// find (;) and erase
	std::size_t found = JsonData.find(";");
	if(found!= std::string::npos) { JsonData.erase(found); }

}

void ConfigFileReader::writeDataToFile()
{
	std::ofstream outfile (CONFIGPATH_OUT);
	if (outfile.is_open())
	{
		std::cerr << "writing to file" << std::endl;
		outfile << JsonData; 
		outfile.close();
	}
	else std::cerr << "unable to open file to write" << std::endl;
 
}


void  ConfigFileReader::parseData()
{
	Jzon::Node rootNode = _filereader.parseString(JsonData);

	ProjectName = rootNode.get("name").toString();
	ShortProjectName = rootNode.get("name_short").toString();
	
	VersionMajor = rootNode.get("version").get("major").toInt(); 
	VersionMinor = rootNode.get("version").get("minor").toInt();
	VersionRevision = rootNode.get("version").get("revision").toInt();

	SpeakerName = rootNode.get("name_speaker").toString();
	WebsiteName = rootNode.get("name_website").toString();

	BrokerToUse = rootNode.get("use_broker").toInt();

	for (int i = 0; i < NUMBEROFBROKERS; ++i)
	{
		std::string id = std::to_string(i+1);
		brokers[i].name = rootNode.get("broker").get(id).get("broker").toString();
		brokers[i].uri 	= rootNode.get("broker").get(id).get("uri").toString();
		brokers[i].port = rootNode.get("broker").get(id).get("port").toInt();
		brokers[i].port_encrypted = rootNode.get("broker").get(id).get("port_encrypted").toInt();
		brokers[i].port_encrypted_certificate =  rootNode.get("broker").get(id).get("port_encrypted_certificate").toInt();
		brokers[i].port_ws = rootNode.get("broker").get(id).get("port_ws").toInt();
		brokers[i].port_ws_encrypted = rootNode.get("broker").get(id).get("port_ws_encrypted").toInt();
	}
}

void ConfigFileReader::printJsonData()
{
	std::cerr << "JsonData: " << std::endl
			  << JsonData << std::endl;
}

std::string ConfigFileReader::getProjectName(bool fullname)
{
	return fullname ? ProjectName : ShortProjectName;
}

std::string ConfigFileReader::getVersion()
{
	return "( " + std::to_string(VersionMajor) + 
			", " + std::to_string(VersionMinor) + 
			", " + std::to_string(VersionRevision) + 
			" )";
}

std::string ConfigFileReader::getSpeakerName()
{
	return SpeakerName;
}

std::string ConfigFileReader::getWebsiteName()
{
	return WebsiteName;
}

Broker ConfigFileReader::getBroker(int broker)
{
	return brokers[broker-1];
}

void ConfigFileReader::printBroker(int broker)
{
	std::cerr 	<< "Broker id = " << broker << std::endl
				<< "Name = " << brokers[broker-1].name << std::endl
				<< "Uri = "	<< brokers[broker-1].uri 	<< std::endl
				<< "Port = " << brokers[broker-1].port << std::endl
				<< "Port encrypted = " << brokers[broker-1].port_encrypted << std::endl
				<< "port encrypted certificate = " << brokers[broker-1].port_encrypted_certificate << std::endl
				<< "port ws = "	<< brokers[broker-1].port_ws << std::endl
				<< "port ws encrypted = " << brokers[broker-1].port_ws_encrypted << std::endl
				<< std::endl;
}