#include "DNSDataParser.hpp"

#include <iostream>
#include <sstream>
#include <string>

DNSDataParser::DNSDataParser () : _distance{ 0 }, _angle{ 0 } {
}

DNSDataParser::~DNSDataParser () {
}


void DNSDataParser::readDataFromString (std::string jsonstring) {
    Jzon::Node tempNode = _filereader.parseString (jsonstring);

    _speakerid = tempNode.get ("speakerid").toInt ();
    _distance  = tempNode.get ("distance").toInt ();
    _angle     = tempNode.get ("angle").toInt ();
}
