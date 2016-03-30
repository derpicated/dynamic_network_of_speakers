#include "DNSMusic.h"

DNSMusic::DNSMusic (const std::string& appname,
const std::string& clientname,
const std::string& host,
int port,
const std::string musicfile)
: mosqpp::mosquittopp{ (CLIENT_XXX + appname + clientname).c_str () },
  _appname{ appname }, _clientname{ clientname }, _musicfile{ musicfile },
  _topicRoot{ "ESEiot" }, _speaker_data (), _distances{}, _volume{ 0 },
  _stop{ true }, _play{ false }, _pause{ false }, _jsondatastring{ "" }, _cv{},
  _mtx{}, _running{ true },
  /*_thread_data{ &DNSMusic::processClientData, this },
  _thread_music{ &DNSMusic::MusicPlayer, this },*/ _player (musicfile)

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
    stop ();
    disconnect ();
}

void DNSMusic::stop () {
    _running = false;
    /*if (_thread_music.joinable ()) {
        _thread_music.join ();
    }
    if (_thread_data.joinable ()) {
        _thread_data.join ();
    }*/
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
    // while (_running) {
    // TODO pass semaphore?
    speakerData _speaker_data = _data_parser.parseClientData (_jsondatastring);

    std::cerr << "distance: " << _speaker_data.distance << " "
              << "angle: " << _speaker_data.angle << " "
              << "speakerid" << _speaker_data.speakerid << std::endl;

    // relative_weight_factor::rwf<int> rwf_int ();
    D (std::cerr << "this is the thread for calculating the RWF"
                 << "Client name: " << CLIENT_XXX << std::endl;);
    D (std::cerr << "JsonDataString" << _jsondatastring << std::endl;);
    //}
}

/*void DNSMusic::MusicPlayer () {
    std::chrono::milliseconds Tms{ 1000 };
    std::chrono::milliseconds Tstartup{ 5000 };
    std::this_thread::sleep_for (Tstartup);
    bool playing = false;

    while (_running) {
        if (playing == false) {
            if (_play == true) {
                _player.play ();
                playing = true;
            }
        }
        if (playing == true) {
            if (_stop == true) {
                _player.stop ();
                playing = false;
            }
        }
        D (std::cerr << "this the thread for playing music " << std::endl
                     << "volume = " << std::setprecision (3) << _volume <<
std::endl
                     << "Play " << _play << "  "
                     << "Pause " << _pause << " "
                     << "Stop " << _stop << std::endl;);
        std::this_thread::sleep_for (Tms);
    }
}*/
