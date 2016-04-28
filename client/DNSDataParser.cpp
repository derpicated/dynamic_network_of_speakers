#include "DNSDataParser.hpp"

audioObject::audioObject ()
: distance (0)
, angle (0) {
}

speakerData::speakerData ()
: speakerid ("")
, objects{} {
}

dataParser::dataParser ()
: _filereader ()
, _filewriter () {
}

speakerData dataParser::parseClientData (std::string jsonstring,
std::map<std::string, std::vector<float>>& objects) {
    Jzon::Node rootNode = _filereader.parseString (jsonstring);
    speakerData ret_speaker;

    for (Jzon::NamedNode node : rootNode) {
        if (node.first == CLIENT_XXX) {

            ret_speaker.speakerid = node.first;
            for (Jzon::NamedNode sub_node : node.second) {
                audioObject object;
                std::string key          = sub_node.first;
                object.distance          = sub_node.second.get ("distance").toFloat ();
                object.angle             = sub_node.second.get ("angle").toFloat ();
                ret_speaker.objects[key] = object;
            }

        } else {
            for (Jzon::NamedNode sub_node : node.second) {
                std::string key = sub_node.first;
                int distance = sub_node.second.get ("distance").toInt ();
                if (distance != -1) {
                    objects[key].push_back (distance);
                }
            }
        }
    }
    return ret_speaker;
}


std::map<std::string, std::string> dataParser::parseAudioSourceData (std::string jsonstring) {
    std::map<std::string, std::string> ret;
    Jzon::Node root_node = _filereader.parseString (jsonstring);

    for (Jzon::NamedNode sub_node : root_node) {
        std::string uri, name;
        name = sub_node.first;
        uri  = sub_node.second.toString ();

        if (30 < name.length ()) { // truncate string to MAX 30
            name.erase (30, std::string::npos);
        }
        for (char& c : name) { // replace any non alphanumerical characters
            if (!isalnum (c)) {
                c = '_';
            }
        }

        ret[name] = uri;
    }

    return ret;
}

std::string dataParser::composeClientData (speakerData speaker) {
    std::string ret_str;
    Jzon::Node root_node, speaker_node, object_node;

    for (auto object : speaker.objects) {
        object_node.add ("distance", object.second.distance);
        object_node.add ("angle", object.second.angle);
        speaker_node.add (object.first, object_node);
    }
    root_node.add (speaker.speakerid, speaker_node);
    _filewriter.writeString (root_node, ret_str);
    return ret_str;
}
