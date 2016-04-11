#include "DNSMusic.h"

DNSMusic::DNSMusic (const std::string& appname,
const std::string& clientname,
const std::string& host,
int port,
const std::string musicfile)
: mosqpp::mosquittopp{ (CLIENT_XXX + appname + clientname).c_str () }
, _appname{ appname }
, _clientname{ clientname }
, _musicfile{ musicfile }
, _topicRoot{ "ESEiot" }
, _distances{}
, _volume{ 0 }
, _stop{ true }
, _play{ false }
, _pause{ false }
, _jsondatastring{ "" }
, _mtx{}
, _cache_path (".cache/")
, _player (musicfile)
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

    std::cerr << "Value of the struct mosquitto_message" << std::endl
              << "Mid: " << message->mid << std::endl
              << "topic: " << message->topic << std::endl
              << "payload: " << (char*)message->payload
              << " lengte: " << message->payloadlen << std::endl
              << "QOS: " << message->qos << std::endl
              << "Retian" << message->retain << std::endl
              << std::endl;

    if (topic.compare (MQTT_TOPIC_REQUEST_ONLINE) == 0) {
        publish (nullptr, MQTT_TOPIC_INFO_CLIENT_ONLINE.c_str (),
        CLIENT_XXX.size (), CLIENT_XXX.c_str (), MQTT_QoS_0);
    }

    if (topic.compare (MQTT_TOPIC_INFO_MUSIC_VOLUME) == 0) {
        setVolume (std::string{ (char*)message->payload });
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

void DNSMusic::setVolume (std::string volume) {
    try {
        size_t size{ 0 };
        _volume = stoi (volume, &size);
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
    _player.set_volume (_volume);
}

void DNSMusic::setPPS (std::string pps) {
    if (!pps.compare ("p") || !pps.compare ("play")) {
        _player.play ();
    }
    if (!pps.compare ("pa") || !pps.compare ("pause")) {
        _player.stop ();
    }
    if (!pps.compare ("s") || !pps.compare ("stop")) {
        _player.stop ();
    }
}

void DNSMusic::processMusicSourceData (std::string json_str) {
    audioSourceData _source_data = _data_parser.parseAudioSourceData (json_str);

    download::download (_source_data.uri, _source_data.name);
}


void DNSMusic::processClientData (std::string json_str) {
    _data_parser.parseClientData (json_str);
    int numberofobjects = _data_parser.getnumberofObjects();

    for (int i = 0; i < numberofobjects; ++i)
    {
      std::cerr << "speakerid: "<< _data_parser.speaker[i].speakerid << std::endl
                << "distance: " << _data_parser.speaker[i].distance  << std::endl
                << "angle: "    << _data_parser.speaker[i].angle     << std::endl;
    }
    // // relative_weight_factor::rwf<int> rwf_int ();
}