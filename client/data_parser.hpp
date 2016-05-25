#ifndef DNS_data_parser_HPP
#define DNS_data_parser_HPP

#include "./libs/logger/easylogging++.h"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "libs/jzon/Jzon.h"


class audioObject {
    public:
    audioObject ();

    float distance;
    float angle;
};

class speakerData {
    public:
    speakerData ();

    std::string speakerid;
    std::map<std::string, audioObject> objects;
};

class data_parser {
    private:
    Jzon::Parser _filereader;
    Jzon::Writer _filewriter;

    public:
    data_parser ();
    speakerData parseClientData (std::string jsonstring,
    std::string client_id,
    std::map<std::string, std::vector<float>>& objects);

    std::map<std::string, std::string> parseAudioSourceData (std::string jsonstring);

    std::string composeClientData (speakerData speaker);
    std::string composeAudioSourceData (std::map<std::string, std::string> audioSources);
};

#endif
