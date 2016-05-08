#include "DNSMusic.h"

DNSMusic::DNSMusic (const std::string& appname,
const std::string& clientname,
const std::string& clientid)
: mosqpp::mosquittopp{ (clientid).c_str () }
, _appname{ appname }
, _clientname{ clientname }
, _topicRoot{ "ESEiot" }
, _mtx{}
, _client_id (clientid)
, _cache_path (".cache/")
, _master_volume (0)
, _sources ()
, _rwf_volumes ()
, _players ()
, _speaker_data ()
, _data_parser ()

{
    will_set (MQTT_TOPIC_INFO_CLIENTS_OFFLINE.c_str (), _client_id.size (),
    _client_id.c_str (), MQTT_QoS_0);

    connect (MQTT_BROKER.c_str (), MQTT_BROKER_PORT, MQTT_KEEP_ALIVE);
}

DNSMusic::~DNSMusic () {
    std::cerr << "---- ** disconnecting DNSMusic" << std::endl;
    publish (nullptr, MQTT_TOPIC_INFO_CLIENTS_OFFLINE.c_str (),
    _client_id.size (), _client_id.c_str (), MQTT_QoS_0);
    disconnect ();
}

void DNSMusic::on_connect (int rc) {
    if (rc == 0) {
        publish (nullptr, MQTT_TOPIC_INFO_CLIENTS_ONLINE.c_str (),
        _client_id.size (), _client_id.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_REQUEST_ONLINE.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_REQUEST_INFO_CLIENTS.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_INFO_CLIENTS_DATA_WILDCARD.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_INFO_MUSIC_VOLUME.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_INFO_MUSIC_STATUS.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_INFO_MUSIC_SOURCES.c_str (), MQTT_QoS_0);
    }
}

void DNSMusic::on_disconnect (int rc) {
    if (!(rc == 0)) {
        publish (nullptr, MQTT_TOPIC_INFO_CLIENTS_OFFLINE.c_str (),
        _client_id.size (), _client_id.c_str (), MQTT_QoS_0);
    }
    std::cerr << "---- DNSMusic disconnected with rc = " << rc << std::endl;
}

void DNSMusic::on_message (const mosquitto_message* message) {
    std::unique_lock<std::mutex> lk{ _mtx };
    std::string topic{ message->topic };

    D (std::cerr << "DATA received by a topic + message: " << topic << " "
                 << (char*)message->payload << std::endl;);

    D (std::cerr << "Value of the struct mosquitto_message" << std::endl
                 << "Mid: " << message->mid << std::endl
                 << "topic: " << message->topic << std::endl
                 << "payload: " << (char*)message->payload
                 << " lengte: " << message->payloadlen << std::endl
                 << "QOS: " << message->qos << std::endl
                 << "Retian" << message->retain << std::endl
                 << std::endl;);

    if (topic == MQTT_TOPIC_REQUEST_ONLINE) {
        publish (nullptr, MQTT_TOPIC_INFO_CLIENTS_ONLINE.c_str (),
        _client_id.size (), _client_id.c_str (), MQTT_QoS_0);
    }

    if (topic == MQTT_TOPIC_REQUEST_INFO_CLIENTS) {
        const std::string str_payload = _data_parser.composeClientData (_speaker_data);
        publish (nullptr, (char*)message->payload, str_payload.size (),
        str_payload.c_str (), MQTT_QoS_0);
    }

    if (topic == MQTT_TOPIC_INFO_MUSIC_VOLUME) {
        setMasterVolume (std::string{ (char*)message->payload });
    }

    if (topic == MQTT_TOPIC_INFO_MUSIC_STATUS) {
        setPPS (std::string{ (char*)message->payload });
    }

    if (topic == MQTT_TOPIC_INFO_MUSIC_SOURCES) {
        processMusicSourceData (std::string{ (char*)message->payload });
    }

    if (topic.find (MQTT_TOPIC_INFO_CLIENTS_DATA) == 0) {
        processClientData (std::string{ (char*)message->payload });
    }
}

void DNSMusic::on_subscribe (int mid, int qos_count, const int* granted_qos) {
    std::cerr << "---- DNSMusic subscription succeeded mid = " << mid << std::endl
              << " qos_count = " << qos_count << std::endl
              << " granted_qos = " << *granted_qos << std::endl;
}


void DNSMusic::on_log (int level, const char* str) {
    std::cerr << "---- # log DNSMusic " << level << std::endl
              << ": " << str << std::endl;
}

void DNSMusic::on_error () {
    std::cerr << "**** DNSMusic ERROR" << std::endl;
}

void DNSMusic::setMasterVolume (std::string volume) {
    try {
        size_t size{ 0 };
        _master_volume = stoi (volume, &size);
        if (volume.size () != size) {
            D (std::cerr << "wrong volume value" << std::endl;);
        }
    } catch (std::invalid_argument& e) {
        D (std::cerr << std::endl
                     << "## EXCEPTION s40 invalid_argument: " << e.what () << std::endl;);
    } catch (std::out_of_range& e) {
        D (std::cerr << std::endl
                     << "## EXCEPTION s40 out_of_range: " << e.what () << std::endl;);
    }

    for (auto& object : _players) {
        int rwf_volume, adjusted_volume;

        rwf_volume      = _rwf_volumes[object.first];
        adjusted_volume = (rwf_volume * _master_volume) / 100;

        object.second.set_volume (adjusted_volume);
    }
}

void DNSMusic::setPPS (std::string status_str) {
    enum play_status { play, pause, stop } status;
    if (status_str == "p" || status_str == "play") {
        status = play;
    }
    if (status_str == "pa" || status_str == "pause") {
        status = pause;
    }
    if (status_str == "s" || status_str == "stop") {
        status = stop;
    }

    for (auto& player : _players) {
        switch (status) {
            case play: player.second.play (); break;
            case pause: // simply stop playing
            case stop: player.second.stop (); break;
        }
    }
}

/*
new sources come in

check if new sources are in old sources map (by name)
    if so, compare uri
        if uri is same, do nothing
        if uri is different
            if object has a player
                stop player
                download file
                if was playing before stopped
                    start playing
            else
                download file

if source is new
    add to sources
    download
beepboop
*/
void DNSMusic::processMusicSourceData (std::string json_str) {
    std::map<std::string, std::string> new_sources;
    new_sources = _data_parser.parseAudioSourceData (json_str);

    for (auto new_source : new_sources) {
        std::string local_name = new_source.first;

        if (_sources[local_name] != new_source.second) {
            // only interact with player if it already exists
            if (_players.find (local_name) != _players.end ()) {
                int status = _players[local_name].stop ();

                download::download (new_source.second, _cache_path + local_name);

                // only start playing if it was previously playing
                if (status == 0) {
                    _players[local_name].set_file (_cache_path + local_name);
                    _players[local_name].play ();
                }
            } else {
                download::download (new_source.second, _cache_path + local_name);
            }
        }
    }
    _sources = new_sources;
}

/*
new objects come in
parse into map of vectors of speaker distances
parse local speaker objects

for every local speaker object
    get vector of speaker distances to said object
    append vector with personal distance
    calculate rwf from vector
    get local object volume from rwf returns
    adjust local object volume to master volume
    if audio player exists for object
        set player local object volume
    else
        if a source file is known for the object
            create player and set file
            set player local object volume
*/

void DNSMusic::processClientData (std::string json_str) {
    std::map<std::string, std::vector<float>> objects;
    float rwf_volume, adjusted_volume;
    rwf::rwf<float> rwf ({ 0 });
    _speaker_data = _data_parser.parseClientData (json_str, _client_id, objects);

    for (auto audio_object : _speaker_data.objects) {
        std::string object_name      = audio_object.first;
        std::vector<float> distances = objects[object_name];
        distances.push_back (audio_object.second.distance);

        rwf.set_factors (distances);
        rwf_volume = rwf.get_relative_weight_factor ().back ();

        _rwf_volumes[object_name] = rwf_volume;
        adjusted_volume           = (rwf_volume * _master_volume) / 100;

        if (_players.find (object_name) != _players.end ()) {
            _players[object_name].set_volume (adjusted_volume);
        } else {
            if (_sources.find (object_name) != _sources.end ()) {
                _players[object_name].set_file (_cache_path + object_name);
                _players[object_name].set_volume (adjusted_volume);
            } else {
                ; // TODO LOG this somehow: no file known for this object
            }
        }
    }

    // remove all unneeded players
    for (auto it = _players.cbegin (); it != _players.cend ();) {
        if (_speaker_data.objects.find (it->first) == _speaker_data.objects.end ()) {
            _players.erase (it++);
        } else {
            ++it;
        }
    }
}
