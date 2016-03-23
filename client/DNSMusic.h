#ifndef DNS_MUSIC_H
#define DNS_MUSIC_H

#include "Topic.h"
#include "../libs/rwf/relative_weight_factor.hpp"
#include "audio.hpp"

#include <mosquittopp.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <map>
#include <string>
#include <thread>
#include <utility>
#include <stdlib.h>



class DNSMusic : public mosqpp::mosquittopp
{
public:
  DNSMusic(	const std::string& appname,
           	const std::string& clientname,
           	const std::string& host,
           	int port);
  DNSMusic(const DNSMusic& other) = delete;
  DNSMusic& operator=(const DNSMusic& other) = delete;
  virtual ~DNSMusic();
  void stop();

protected:
  std::string _appname;
  std::string _clientname;
  Topic _topicRoot;
  std::map<std::string, std::pair<std::chrono::system_clock::time_point, int>> _dataStore;

  float Position, Angle;
  int Volume;
  bool Stop, Play, Pause;
  std::string JsonDataString;

  std::condition_variable _cv;
  std::mutex _mtx;
  std::atomic<bool> _running;
  std::thread _thread_data, _thread_music;
 


  // C++11 override (compiler check)
  virtual void on_connect(int rc) override;
  virtual void on_disconnect(int rc) override;
  virtual void on_message(const struct mosquitto_message *message) override;
  virtual void on_subscribe(int mid, int qos_count, const int *granted_qos) override;
  virtual void on_log(int level, const char *str) override;
  virtual void on_error() override;


private:
  void processData();
  void MusicPlayer(); 
};

//,   audio_player {("/media/brian/Downloads/Spotnet/MUZIEK/Mumford & Sons - Wilder MInd (Deluxe Edition) 2015")}

#endif
