#ifndef DNS_DATAPARSER_HPP
#define DNS_DATAPARSER_HPP

#include <iostream>
#include <sstream>
#include <string>

#include "Jzon.h"

class speakerData {
    public:
    speakerData ();

    int speakerid;
    int distance;
    int angle;
};

class audioSourceData {
    public:
    audioSourceData ();

    std::string name;
    std::string uri;
};

class dataParser {
    Jzon::Parser _filereader;

    public:
    speakerData parseClientData (std::string jsonstring);
    audioSourceData parseAudioSourceData (std::string jsonstring);
};

#endif
