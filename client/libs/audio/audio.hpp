#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <signal.h>
#include <string>
#include <unistd.h>
#include <wait.h>

class audio_player {
    std::string file_name;
    pid_t child_pid;

    void call_player (unsigned int time);
    void call_mixer (unsigned int volume);

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
