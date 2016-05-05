#ifndef DNS_MUSIC_H
#define DNS_MUSIC_H

#include <assert.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <mosquittopp.h>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <thread>
#include <utility>

#include "Config.h"
#include "DNSDataParser.hpp"
#include "libs/audio/audio.hpp"
#include "libs/download/download.hpp"
#include "libs/rwf/relative_weight_factor.hpp"
#include "libs/topic/Topic.h"

/* Debug MACRO */
//#define MAIN_DEBUG
#ifdef MAIN_DEBUG
#define D(call) (call)
#else
#define D(call)
#endif

class DNSMusic : public mosqpp::mosquittopp {
    public:
    DNSMusic (const std::string& appname, const std::string& clientname, const std::string& clientid);

    DNSMusic (const DNSMusic& other) = delete;
    DNSMusic& operator=(const DNSMusic& other) = delete;
    virtual ~DNSMusic ();

    protected:
    std::string _appname;
    std::string _clientname;
    // std::string _musicfile;
    Topic _topicRoot;

    std::vector<int> _distances;

    std::string _jsondatastring;
    std::mutex _mtx;

    // C++11 override (compiler check)
    virtual void on_connect (int rc) override;
    virtual void on_disconnect (int rc) override;
    virtual void on_message (const struct mosquitto_message* message) override;
    virtual void on_subscribe (int mid, int qos_count, const int* granted_qos) override;
    virtual void on_log (int level, const char* str) override;
    virtual void on_error () override;

    private:
    const std::string _client_id; //{ getClientID () };
    const std::string _cache_path;
    int _master_volume;
    std::map<std::string, int> _rwf_volumes;      // key = object name
    std::map<std::string, audio_player> _players; // key = object name
    speakerData _speaker_data;
    dataParser _data_parser;

    void setMasterVolume (std::string volume);
    void setPPS (std::string pps);
    void processMusicSourceData (std::string json_str);
    void processClientData (std::string json_str);
    void MusicPlayer ();
};

#endif
