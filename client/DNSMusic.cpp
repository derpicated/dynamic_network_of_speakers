#include "DNSMusic.h"

DNSMusic::DNSMusic (const std::string& appname,
const std::string& clientname,
const std::string& host,
int port)
: mosqpp::mosquittopp{ (CLIENT_XXX + appname + clientname).c_str () }
, _appname{ appname }
, _clientname{ clientname }
, _topicRoot{ "ESEiot" }
, _distances{}
, _stop{ true }
, _play{ false }
, _pause{ false }
, _jsondatastring{ "" }
, _mtx{}
, _cache_path (".cache/")
, _master_volume (0)
, _rwf_volumes ()
, _players ()
, _speaker_data ()
, _data_parser ()

{
    _topicRoot.add ("DNS");

    will_set (MQTT_TOPIC_INFO_CLIENT_OFFLINE.c_str (), CLIENT_XXX.size (),
    CLIENT_XXX.c_str (), MQTT_QoS_0);

    connect (host.c_str (), port, MQTT_KEEP_ALIVE);
}

DNSMusic::~DNSMusic () {
    std::cerr << "---- ** disconnecting DNSMusic" << std::endl;
    publish (nullptr, MQTT_TOPIC_INFO_CLIENT_OFFLINE.c_str (),
    CLIENT_XXX.size (), CLIENT_XXX.c_str (), MQTT_QoS_0);
    disconnect ();
}

void DNSMusic::on_connect (int rc) {
    if (rc == 0) {
        publish (nullptr, MQTT_TOPIC_INFO_CLIENT_ONLINE.c_str (),
        CLIENT_XXX.size (), CLIENT_XXX.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_REQUEST_ONLINE.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_CLIENTID_OBJECTID.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_INFO_MUSIC_VOLUME.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_INFO_MUSIC_PS.c_str (), MQTT_QoS_0);
        subscribe (nullptr, MQTT_TOPIC_INFO_MUSIC_SOURCE.c_str (), MQTT_QoS_0);
    }
}

void DNSMusic::on_disconnect (int rc) {
    if (!(rc == 0)) {
        publish (nullptr, MQTT_TOPIC_INFO_CLIENT_OFFLINE.c_str (),
        CLIENT_XXX.size (), CLIENT_XXX.c_str (), MQTT_QoS_0);
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
        publish (nullptr, MQTT_TOPIC_INFO_CLIENT_ONLINE.c_str (),
        CLIENT_XXX.size (), CLIENT_XXX.c_str (), MQTT_QoS_0);
    }

    if (topic.compare (MQTT_TOPIC_REQUEST_INFORMATION_CLIENT) == 0) {
        const char* cstr_payload =
        _data_parser.composeClientData (_speaker_data).c_str ();
        publish (nullptr, (char*)message->payload, strlen (cstr_payload),
        cstr_payload, MQTT_QoS_0);
    }

    if (topic.compare (MQTT_TOPIC_INFO_MUSIC_VOLUME) == 0) {
        setMasterVolume (std::string{ (char*)message->payload });
    }

    if (topic.compare (MQTT_TOPIC_INFO_MUSIC_PS) == 0) {
        setPPS (std::string{ (char*)message->payload });
    }

    if (topic.compare (MQTT_TOPIC_INFO_MUSIC_SOURCE) == 0) {
        processMusicSourceData (std::string{ (char*)message->payload });
    }

    if (topic.compare (MQTT_TOPIC_CLIENTID_OBJECTID) == 0) {
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

    for (auto player : _players) {
        switch (status) {
            case play: player.second.play ();
            case pause: player.second.stop ();
            case stop: player.second.stop ();
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
    _speaker_data = _data_parser.parseClientData (json_str, objects);

    for (auto object : objects) {
        std::vector<float> distances = object.second;
        distances.push_back (_speaker_data.objects[object.first].distance);

        rwf.set_factors (distances);
        rwf_volume = rwf.get_relative_weight_factor ().back ();

        _rwf_volumes[object.first] = rwf_volume;
        adjusted_volume            = (100 * rwf_volume) / _master_volume;
        _players[object.first].set_volume (adjusted_volume);
    }
    // // relative_weight_factor::rwf<int> rwf_int ();
}
