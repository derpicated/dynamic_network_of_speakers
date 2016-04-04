#include "DNSDataParser.hpp"

audioSourceData::audioSourceData ()
: name ("")
, uri ("")
{}


dataParser::dataParser()
: numberofObject(0) 
, _filereader ()
{}

void dataParser::parseClientData (std::string jsonstring) {
    Jzon::Node rootNode = _filereader.parseString (jsonstring);

    numberofObject = rootNode.get("objects").toInt();
    std::string objectid {"objectid0"};

    for(int i = 0; i< numberofObject; ++i){
        objectid = objectid.replace(8,1,std::to_string(i+1));
        speaker[i].speakerid = rootNode.get(objectid).get("speakerid").toString();
        speaker[i].distance  = rootNode.get(objectid).get("distance").toFloat();
        speaker[i].angle     = rootNode.get(objectid).get("angle").toFloat();                                          
    }
}

int dataParser::getnumberofObjects()
{
  return numberofObject;
}

audioSourceData dataParser::parseAudioSourceData (std::string jsonstring) {
    audioSourceData ret;
    Jzon::Node tempNode = _filereader.parseString (jsonstring);

    ret.uri  = tempNode.get ("uri");
    ret.name = tempNode.get ("name");
    return ret;
}
