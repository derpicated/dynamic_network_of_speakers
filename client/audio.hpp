#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <string>

class audio_player{
    std::string file_name;
    pid_t child_pid;
    const std::string player_cmd = "mpg123";
    const std::string mixer_cmd = "amixer";
    const std::string mixer_options = "-q sset Master";

public:
   audio_player(std::string file_name);
   void play(unsigned int time = 0);
   void stop();
   void set_volume(unsigned int volume);
};

#endif //AUDIO_HPP
