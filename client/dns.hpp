#ifndef DNS_H
#define DNS_H

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

#include "data_parser.hpp"
#include "libs/audio/audio.hpp"
#include "libs/config/config_parser.hpp"
#include "libs/download/download.hpp"
#include "libs/logger/easylogging++.h"
#include "libs/rwf/relative_weight_factor.hpp"
#include "libs/topic/Topic.h"

class dns : public mosqpp::mosquittopp {
    public:
    const int MQTT_QoS_0{ 0 };
    const int MQTT_QoS_1{ 1 };
    const int MQTT_QoS_2{ 2 };
    const bool MQTT_RETAIN_OFF{ false };
    const bool MQTT_RETAIN_ON{ true };
    const int MQTT_KEEP_ALIVE{ 60 };
    dns (config_parser& config);

    dns (const dns& other) = delete;
    dns& operator= (const dns& other) = delete;
    virtual ~dns ();

    protected:
    Topic _topicRoot;
    std::mutex _mtx;

    virtual void on_connect (int rc) override;
    virtual void on_disconnect (int rc) override;
    virtual void on_message (const struct mosquitto_message* message) override;
    virtual void on_subscribe (int mid, int qos_count, const int* granted_qos) override;
    virtual void on_log (int level, const char* str) override;
    virtual void on_error () override;

    private:
    config_parser& CONFIG;
    const std::string _cache_path;
    int _master_volume;
    std::map<std::string, std::string> _sources;
    std::map<std::string, int> _rwf_volumes;      // key = object name
    std::map<std::string, audio_player> _players; // key = object name
    speakerData _speaker_data;
    data_parser _data_parser;

    void setMasterVolume (std::string volume);
    void setPPS (std::string pps);
    void processMusicSourceData (std::string json_str);
    void processClientData (std::string json_str);
    void MusicPlayer ();
};

#endif
