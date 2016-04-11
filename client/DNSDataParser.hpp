#ifndef DNS_DATAPARSER_HPP
#define DNS_DATAPARSER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
/*look at this headerfile*/

#include "libs/jzon/Jzon.h"

struct speakerData {
    std::string speakerid;
    float distance;
    float angle;
    
};

class audioSourceData {
public:
    audioSourceData ();

    std::string name;
    std::string uri;
};

class dataParser {
private:
    int numberofObject;
    Jzon::Parser _filereader;
public:
    dataParser();
    int getnumberofObjects();
    void parseClientData (std::string jsonstring);
    audioSourceData parseAudioSourceData (std::string jsonstring);

    speakerData speaker[10];
};

#endif

// jzon string 
// {
//      "objects": 3 ,
//      "objectid1":{
//         "speakerid":  "speaker010", 
//         "distance": 10, 
//         "angle": 20
//         },
//      "objectid2":{
//         "speakerid":  "speaker020", 
//         "distance": 30, 
//         "angle": 40
//         },
//      "objectid3":{
//         "speakerid":  "speaker030", 
//         "distance": 50, 
//         "angle": 60
//         },
//  }