#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "../client/ConfigFileReader.hpp"

int main (int argc, char const* argv[]) 
{
	ConfigFileReader test_configfilereader;

	//test_configfilereader.writeDataToFile();

	std::cerr << "ProjectName = " << test_configfilereader.getProjectName() << std::endl;
	std::cerr << "Version = " << test_configfilereader.getVersion()		<< std::endl;
	std::cerr << "Speaker Name = " << test_configfilereader.getSpeakerName() << std::endl;
	std::cerr << "Website Name = " << test_configfilereader.getWebsiteName() << std::endl;
	std::cerr << std::endl;

	test_configfilereader.printBroker(1); 
	test_configfilereader.printBroker(2);
	test_configfilereader.printBroker(3); 
	test_configfilereader.printBroker(4);

}