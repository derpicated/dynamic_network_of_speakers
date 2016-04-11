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
    DNSMusic (const std::string& appname,
    const std::string& clientname,
    const std::string& host,
    int port,
    const std::string musicfile);

    DNSMusic (const DNSMusic& other) = delete;
    DNSMusic& operator= (const DNSMusic& other) = delete;
    virtual ~DNSMusic ();
    void stop ();

    protected:
    std::string _appname;
    std::string _clientname;
    std::string _musicfile;
    Topic _topicRoot;
    dataParser _data_parser;
    audioSourceData _speaker_data;
    std::vector<int> _distances;

    int _volume;
    bool _stop, _play, _pause;
    std::string _jsondatastring;

    std::condition_variable _cv;
    std::mutex _mtx;
    std::atomic<bool> _running;
    // std::thread _thread_data, _thread_music;


    // C++11 override (compiler check)
    virtual void on_connect (int rc) override;
    virtual void on_disconnect (int rc) override;
    virtual void on_message (const struct mosquitto_message* message) override;
    virtual void on_subscribe (int mid, int qos_count, const int* granted_qos) override;
    virtual void on_log (int level, const char* str) override;
    virtual void on_error () override;


    private:
    const std::string _cache_path;
    audio_player _player;
    void setVolume (std::string svolume);
    void setPPS (std::string pps);
    void processMusicSourceData (std::string json_str);
    void processClientData (std::string json_str);
    void MusicPlayer ();
};

#endif
