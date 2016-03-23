#include "audio.hpp"

audio_player::audio_player(std::string file_name) : file_name(file_name){
}

void audio_player::play(unsigned int time /*= 0*/){
    int status;

    child_pid = fork();
    switch (child_pid)
    {
        case -1: //error
        perror("failed to start audio player");
        exit(1);

        case 0: // child process
        execl("/usr/bin/mpg123", "mpg123", file_name.c_str(), (char*) NULL);
        perror("execl");
        exit(1);

        default: // parent process, pid now contains the child pid
        ;
    }

    return;
}

void audio_player::stop(){
    int status;

    status = kill(child_pid, SIGKILL);

    while (-1 == waitpid(child_pid, &status, 0));
}

void audio_player::set_volume(unsigned int volume){
    int status;
    pid_t pid;
    std::string vol_str = std::to_string(volume)+"%";

    pid = fork();
    switch (pid)
    {
        case -1: //error
        perror("failed to start audio player");
        exit(1);

        case 0: // child process
        execl("/usr/bin/amixer", "amixer", "sset", "Master", vol_str.c_str(), (char*) NULL);
        perror("execl: f");
        exit(1);

        default: // parent process, pid now contains the child pid
        waitpid(pid, &status, 0);
    }

    return;
}
