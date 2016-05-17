#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <signal.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <vector>

#include "../logger/easylogging++.h"

class audio_player {
    std::string _file_name;
    std::string _pa_index;
    pid_t _child_pid;

    void call_player (unsigned int time);
    void call_mixer (unsigned int volume);
    std::string get_pa_index ();
    void get_pa_list (std::vector<std::string>& ret_vec);

    public:
    audio_player ();
    audio_player (std::string file_name);
    ~audio_player ();
    void set_file (std::string file_name);
    void play (unsigned int time = 0);
    int stop ();
    void set_volume (unsigned int volume);
};

#endif // AUDIO_HPP
