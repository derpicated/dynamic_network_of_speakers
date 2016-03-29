#ifndef DNS_MUSIC_H
#define DNS_MUSIC_H

#include "../libs/rwf/relative_weight_factor.hpp"
#include "DNSDataParser.hpp"
#include "Topic.h"
#include "audio.hpp"


#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <map>
#include <mosquittopp.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <utility>


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
    std::map<std::string, std::pair<std::chrono::system_clock::time_point, int>> _dataStore;

    float _distance, _angle;
    int _volume;
    bool _stop, _play, _pause;
    std::string _jsondatastring;


    std::condition_variable _cv;
    std::mutex _mtx;
    std::atomic<bool> _running;
    std::thread _thread_data, _thread_music;


    // C++11 override (compiler check)
    virtual void on_connect (int rc) override;
    virtual void on_disconnect (int rc) override;
    virtual void on_message (const struct mosquitto_message* message) override;
    virtual void on_subscribe (int mid, int qos_count, const int* granted_qos) override;
    virtual void on_log (int level, const char* str) override;
    virtual void on_error () override;


    private:
    audio_player _player;
    DNSDataParser _jsonreader;
    void setVolume (std::string svolume);
    void setPPS (std::string pps);
    void processData ();
    void MusicPlayer ();
};

#endif
