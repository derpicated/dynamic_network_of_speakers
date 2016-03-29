#ifndef DNS_DATAPARSER_HPP
#define DNS_DATAPARSER_HPP

#include "Jzon.h"
#include <string>

struct speakerdata {
    int distance;
    int angle;
};


class DNSDataParser {
    private:
    Jzon::Parser _filereader;

    protected:
    public:
    DNSDataParser ();
    ~DNSDataParser ();

    void readDataFromString (std::string jsonstring);


    /*data variables*/
    int _speakerid;
    int _distance;
    int _angle;
};


#endif
