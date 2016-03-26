#include "DNSDataParser.hpp"

#include <string>
#include <iostream>
#include <sstream>

DNSDataParser::DNSDataParser()
:	_distance {0}
, 	_angle {0}
{

} 

DNSDataParser::~DNSDataParser() {}



void DNSDataParser::readDataFromString(std::string jsonstring)
{
	Jzon::Node tempNode = _filereader.parseString(jsonstring);

	_speakerid  = tempNode.get("speakerid").toInt();
	_distance 	= tempNode.get("distance").toInt();
	_angle 		= tempNode.get("angle").toInt();
}
