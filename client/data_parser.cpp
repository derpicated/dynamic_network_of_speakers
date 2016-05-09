#include "data_parser.hpp"

audioObject::audioObject ()
: distance (0)
, angle (0) {
}

speakerData::speakerData ()
: speakerid ("")
, objects{} {
}

data_parser::data_parser ()
: _filereader ()
, _filewriter () {
}

speakerData data_parser::parseClientData (std::string jsonstring,
std::string client_id,
std::map<std::string, std::vector<float>>& objects) {
    Jzon::Node rootNode = _filereader.parseString (jsonstring);
    speakerData ret_speaker;

    for (Jzon::NamedNode node : rootNode) {
        if (node.first == client_id) {
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


std::map<std::string, std::string> data_parser::parseAudioSourceData (std::string jsonstring) {
    std::map<std::string, std::string> ret;
    Jzon::Node root_node = _filereader.parseString (jsonstring);

    for (Jzon::NamedNode sub_node : root_node) {
        std::string uri, name;
        name = sub_node.first;
        uri  = sub_node.second.toString ();

        if (30 < name.length ()) { // truncate string to MAX 30
            name.erase (30, std::string::npos);
        }

        char last_c = '.'; // set '.' to dissalow the name "."
        for (char& c : name) {
            // dissalow any special characters
            if (!isalnum (c)) {
                // disallow multiple consecutive dots
                if (!(c == '.' && last_c != '.')) {
                    c = '_';
                }
                last_c = c;
            }
        }

        ret[name] = uri;
    }

    return ret;
}

std::string data_parser::composeClientData (speakerData speaker) {
    std::string ret_str;
    Jzon::Node root_node    = Jzon::object ();
    Jzon::Node speaker_node = Jzon::object ();

    for (auto object : speaker.objects) {
        Jzon::Node object_node = Jzon::object ();
        object_node.add ("distance", object.second.distance);
        object_node.add ("angle", object.second.angle);
        speaker_node.add (object.first, object_node);
    }
    root_node.add (speaker.speakerid, speaker_node);
    _filewriter.writeString (root_node, ret_str);
    return ret_str;
}
