#include "DNSMusic.h"

DNSMusic::DNSMusic (const std::string& appname,
const std::string& clientname,
const std::string& clientid)
: mosqpp::mosquittopp{ (clientid).c_str () }
, _appname{ appname }
, _clientname{ clientname }
, _topicRoot{ "ESEiot" }
, _distances{}
, _jsondatastring{ "" }
, _mtx{}
, _client_id (clientid)
, _cache_path (".cache/")
, _master_volume (0)
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
        subscribe (nullptr, MQTT_TOPIC_INFO_CLIENTS_WILDCARD.c_str (), MQTT_QoS_0);
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

    if (topic.compare (MQTT_TOPIC_REQUEST_ONLINE) == 0) {
        publish (nullptr, MQTT_TOPIC_INFO_CLIENTS_ONLINE.c_str (),
        _client_id.size (), _client_id.c_str (), MQTT_QoS_0);
    }

    if (topic.compare (MQTT_TOPIC_REQUEST_INFO_CLIENTS) == 0) {
        const char* cstr_payload =
        _data_parser.composeClientData (_speaker_data).c_str ();
        publish (nullptr, (char*)message->payload, strlen (cstr_payload),
        cstr_payload, MQTT_QoS_0);
    }

    if (topic.compare (MQTT_TOPIC_INFO_MUSIC_VOLUME) == 0) {
        setMasterVolume (std::string{ (char*)message->payload });
    }

    if (topic.compare (MQTT_TOPIC_INFO_MUSIC_STATUS) == 0) {
        setPPS (std::string{ (char*)message->payload });
    }

    if (topic.compare (MQTT_TOPIC_INFO_MUSIC_SOURCES) == 0) {
        processMusicSourceData (std::string{ (char*)message->payload });
    }

    if (topic.compare (MQTT_TOPIC_INFO_CLIENTS_WILDCARD) == 0) {
        _jsondatastring = (char*)message->payload;
        processClientData ((char*)message->payload);
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

    for (auto object : _players) {
        int rwf_volume, adjusted_volume;

        rwf_volume      = _rwf_volumes[object.first];
        adjusted_volume = (100 * rwf_volume) / _master_volume;

        object.second.set_volume (adjusted_volume);
    }
}

void DNSMusic::setPPS (std::string pps) {
    enum play_status { play, pause, stop } status;
    if (!pps.compare ("p") || !pps.compare ("play")) {
        status = play;
    }
    if (!pps.compare ("pa") || !pps.compare ("pause")) {
        status = pause;
    }
    if (!pps.compare ("s") || !pps.compare ("stop")) {
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

void DNSMusic::processMusicSourceData (std::string json_str) {
    std::map<std::string, std::string> sources;
    sources = _data_parser.parseAudioSourceData (json_str);

    for (auto source : sources) {
        std::string local = _cache_path + source.first;
        download::download (source.second, local);

        _players[source.first].set_file (local);
    }
}


void DNSMusic::processClientData (std::string json_str) {
    std::map<std::string, std::vector<float>> objects;
    int rwf_volume, adjusted_volume;
    rwf::rwf<float> rwf ({ 0 });
    _speaker_data = _data_parser.parseClientData (json_str, _client_id, objects);

    for (auto audio_object : _speaker_data.objects) {
        std::vector<float> distances = objects[audio_object.first];

        distances.push_back (audio_object.second.distance);

        rwf.set_factors (distances);
        rwf_volume = rwf.get_relative_weight_factor ().back ();

        _rwf_volumes[audio_object.first] = rwf_volume;
        adjusted_volume = (100 * rwf_volume) / _master_volume;
        _players[audio_object.first].set_volume (adjusted_volume);
    }
}
