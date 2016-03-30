#include "DNSDataParser.hpp"

speakerData::speakerData () : speakerid (0), distance (0), angle (0) {
}

audioSourceData::audioSourceData () : name (""), uri ("") {
}

speakerData dataParser::parseClientData (std::string jsonstring) {
    speakerData ret;
    Jzon::Node tempNode = _filereader.parseString (jsonstring);

    ret.speakerid = tempNode.get ("speakerid").toInt ();
    ret.distance  = tempNode.get ("distance").toInt ();
    ret.angle     = tempNode.get ("angle").toInt ();
    return ret;
}

audioSourceData dataParser::parseAudioSourceData (std::string jsonstring) {
    audioSourceData ret;
    Jzon::Node tempNode = _filereader.parseString (jsonstring);

    ret.uri  = tempNode.get ("uri");
    ret.name = tempNode.get ("name");
    return ret;
}
