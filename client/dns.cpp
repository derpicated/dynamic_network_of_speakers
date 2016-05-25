#include "dns.hpp"

dns::dns (config_parser& config)
: mosqpp::mosquittopp{ (config.clientid ()).c_str () }
, _mtx{}
, CONFIG (config)
, _cache_path ("/tmp/dns_sound_cache/")
, _master_volume (0)
, _sources ()
, _rwf_volumes ()
, _musics ()
, _speaker_data ()
, _data_parser () {
    will_set (CONFIG.topic ("online").c_str (), CONFIG.clientid ().size (),
    CONFIG.clientid ().c_str (), MQTT_QoS_0);

    connect (CONFIG.broker ().uri.c_str (), CONFIG.broker ().port, MQTT_KEEP_ALIVE);

    if (mkdir (_cache_path.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
        if (errno != EEXIST) {
            throw (std::runtime_error ("could not create cache folder" + _cache_path));
        }
    }
}

dns::~dns () {
    publish (nullptr, CONFIG.topic ("offline").c_str (),
    CONFIG.clientid ().size (), CONFIG.clientid ().c_str (), MQTT_QoS_0);
    disconnect ();
}

void dns::on_connect (int rc) {
    LOG (INFO) << "MQTT:\tconnected with broker, rc=" << rc;
    if (rc == 0) {
        publish (nullptr, CONFIG.topic ("online").c_str (),
        CONFIG.clientid ().size (), CONFIG.clientid ().c_str (), MQTT_QoS_0);
        subscribe (nullptr, CONFIG.topic ("request_online").c_str (), MQTT_QoS_0);
        subscribe (nullptr, CONFIG.topic ("request_client_data").c_str (), MQTT_QoS_0);
        subscribe (nullptr, (CONFIG.topic ("clients_data") + "/+").c_str (), MQTT_QoS_0);
        subscribe (nullptr, CONFIG.topic ("music_volume").c_str (), MQTT_QoS_0);
        subscribe (nullptr, CONFIG.topic ("music_status").c_str (), MQTT_QoS_0);
        subscribe (nullptr, CONFIG.topic ("music_sources").c_str (), MQTT_QoS_0);
    }
}

void dns::on_disconnect (int rc) {
    if (!(rc == 0)) {
        publish (nullptr, CONFIG.topic ("offline").c_str (),
        CONFIG.clientid ().size (), CONFIG.clientid ().c_str (), MQTT_QoS_0);
    }
    LOG (ERROR) << "dns disconnected from broker, rc=" << rc;
}

void dns::on_message (const mosquitto_message* message) {
    std::unique_lock<std::mutex> lk{ _mtx };
    std::string topic{ message->topic };

    LOG (DEBUG) << "MQTT:\tmessage: "
                << "topic=" << message->topic << ", payload=" << (char*)message->payload;

    if (topic == CONFIG.topic ("request_online")) {
        publish (nullptr, CONFIG.topic ("online").c_str (),
        CONFIG.clientid ().size (), CONFIG.clientid ().c_str (), MQTT_QoS_0);
    }

    if (topic == CONFIG.topic ("request_client_data")) {
        const std::string str_payload = _data_parser.composeClientData (_speaker_data);
        publish (nullptr, (char*)message->payload, str_payload.size (),
        str_payload.c_str (), MQTT_QoS_0);
    }

    if (topic == CONFIG.topic ("music_volume")) {
        setMasterVolume (std::string{ (char*)message->payload });
    }

    if (topic == CONFIG.topic ("music_status")) {
        setPPS (std::string{ (char*)message->payload });
    }

    if (topic == CONFIG.topic ("music_sources")) {
        processMusicSourceData (std::string{ (char*)message->payload });
    }

    if (topic.find (CONFIG.topic ("clients_data")) == 0) {
        processClientData (std::string{ (char*)message->payload });
    }
}

void dns::on_subscribe (int mid, int qos_count, const int* granted_qos) {
    LOG (DEBUG) << "MQTT:\tsubscription succeeded = " << mid
                << ", qos_count = " << qos_count << ", granted_qos = " << *granted_qos;
}


void dns::on_log (int level, const char* str) {
    LOG (DEBUG) << "MQTT:\tlog level " << level << ": " << str;
}

void dns::on_error () {
    LOG (ERROR) << "MQTT:\tunspecified error";
}

void dns::setMasterVolume (std::string volume) {
    try {
        size_t size{ 0 };
        _master_volume = stoi (volume, &size);
        if (volume.size () != size) {
            LOG (ERROR) << "Attempt to set invalid master volume";
        }
    } catch (std::invalid_argument& e) {
        LOG (ERROR) << "EXCEPTION s40 invalid_argument: " << e.what ();
    } catch (std::out_of_range& e) {
        LOG (ERROR) << "EXCEPTION s40 out_of_range: " << e.what ();
    }

    for (auto& object : _musics) {
        int rwf_volume, adjusted_volume;

        rwf_volume      = _rwf_volumes[object.first];
        adjusted_volume = (rwf_volume * _master_volume) / 100;

        object.second.setVolume (adjusted_volume);
    }
}

void dns::setPPS (std::string status_str) {
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

    for (auto& music : _musics) {
        switch (status) {
            case play: music.second.play (); break;
            case pause: music.second.pause (); break;
            case stop: music.second.stop (); break;
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
void dns::processMusicSourceData (std::string json_str) {
    std::map<std::string, std::string> new_sources;
    new_sources = _data_parser.parseAudioSourceData (json_str);

    for (auto new_source : new_sources) {
        std::string local_name = new_source.first;

        if (_sources[local_name] != new_source.second) {
            // only interact with player if it already exists
            if (_musics.find (local_name) != _musics.end ()) {
                sf::SoundSource::Status status = _musics[local_name].getStatus ();
                _musics[local_name].stop ();

                download::download (new_source.second, _cache_path + local_name);

                // only start playing if it was previously playing
                if (status == sf::SoundSource::Status::Playing) {
                    _musics[local_name].openFromFile (_cache_path + local_name);
                    _musics[local_name].play ();
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

for every player
    if player name is NOT found in local objects
        delete player
    else
        do nothing
*/

void dns::processClientData (std::string json_str) {
    std::map<std::string, std::vector<float>> objects;
    float rwf_volume, adjusted_volume;
    rwf::rwf<float> rwf ({ 0 });
    _speaker_data = _data_parser.parseClientData (json_str, CONFIG.clientid (), objects);

    for (auto audio_object : _speaker_data.objects) {
        std::string object_name      = audio_object.first;
        std::vector<float> distances = objects[object_name];
        distances.push_back (audio_object.second.distance);

        rwf.set_factors (distances);
        rwf_volume = rwf.get_relative_weight_factor ().back ();

        _rwf_volumes[object_name] = rwf_volume;
        adjusted_volume           = (rwf_volume * _master_volume) / 100;

        if (_musics.find (object_name) != _musics.end ()) {
            _musics[object_name].setVolume (adjusted_volume);
        } else {
            if (_sources.find (object_name) != _sources.end ()) {
                _musics[object_name].openFromFile (_cache_path + object_name);
                _musics[object_name].setVolume (adjusted_volume);
            } else {
                LOG (ERROR) << "No known source file for object \"" << object_name << "\"";
            }
        }
    }

    // remove all unneeded players
    for (auto it = _musics.cbegin (); it != _musics.cend ();) {
        if (_speaker_data.objects.find (it->first) == _speaker_data.objects.end ()) {
            _musics.erase (it++);
        } else {
            ++it;
        }
    }
}
