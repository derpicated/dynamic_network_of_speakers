#include "audio.hpp"

#include <iostream>

audio_player::audio_player ()
: _file_name ("")
, _pa_index ("")
, _child_pid ((pid_t)0) {
}

audio_player::audio_player (std::string file_name)
: _file_name (file_name)
, _pa_index ("")
, _child_pid ((pid_t)0) {
}

audio_player::~audio_player () {
    stop ();
}


void audio_player::set_file (std::string file_name) {
    _file_name = file_name;
}

void audio_player::play (unsigned int time /*= 0*/) {
    if (_child_pid == 0) {
        pid_t pid = fork ();
        switch (pid) {
            case -1: // error
                std::perror ("failed to start audio player");
                break;

            case 0: // child process
                call_player (time);
                exit (1);

            default: // parent process, pid now contains the child pid
                _child_pid = pid;
                LOG (DEBUG) << "AUDIO: starting: " << pid;
        }
    }
    return;
}

void audio_player::call_player (unsigned int time) {
    std::string time_str = std::to_string (time);

    execlp ("ogg123", "ogg123", "-q", "-k", time_str.c_str (),
    _file_name.c_str (), (char*)NULL);
    std::perror ("error executing ogg123");
}

int audio_player::stop () {
    int status = -1;
    if (_child_pid != 0) {
        LOG (DEBUG) << "AUDIO: killing: " << _child_pid;
        status     = kill (_child_pid, SIGKILL);
        _child_pid = 0;
        _pa_index.clear ();
    }
    return status;
}

void audio_player::set_volume (unsigned int volume) {
    if (_child_pid != 0) {
        if (_pa_index.empty ()) {
            _pa_index = get_pa_index ();
        }

        LOG (DEBUG) << "AUDIO: setting volume at index: " << _pa_index
                    << " to: " << volume << std::endl;

        pid_t pid = fork ();
        switch (pid) {
            case -1: // error
                std::perror ("failed to start audio player");
                break;

            case 0: // child process
                call_mixer (volume);
                exit (1); // if fails, exit the process

            default: // parent process, pid now contains the child pid
                     ;
        }
    }
}

void audio_player::call_mixer (unsigned int volume) {
    std::string vol_str = std::to_string (volume) + "%";
    execlp ("pactl", "pactl", "set-sink-input-volume", _pa_index.c_str (),
    vol_str.c_str (), (char*)NULL);
    std::perror ("error executing amixer");
}

std::string audio_player::get_pa_index () {
    std::string ret_str;
    // the line to look for in the list:
    std::string line_pid =
    "application.process.id = \"" + std::to_string (_child_pid) + "\"";
    std::vector<std::string> pa_list;
    get_pa_list (pa_list);

    std::string temp_index;
    for (auto line : pa_list) {
        if (line.find ("Sink Input #") != std::string::npos) {
            temp_index = line.substr (line.find ('#') + 1);
            temp_index.pop_back ();
        }
        if (line.find (line_pid) != std::string::npos) {
            ret_str = temp_index;
        }
    }
    return ret_str;
}

void audio_player::get_pa_list (std::vector<std::string>& ret_vec) {
    int pipe_fd[2];
    if (pipe (pipe_fd) == -1) {
        perror ("pipe() failed");
    }
    pid_t pid = fork ();
    switch (pid) {
        case -1: // error
            std::perror ("failed to start audio player");
            break;

        case 0: // child process
            close (pipe_fd[0]);
            if (dup2 (pipe_fd[1], 1) == -1) { // redirect stdout
                perror ("dup2() failed");
            } else {
                execlp ("pactl", "pactl", "list", "sink-inputs", (char*)NULL);
            }
            exit (1); // if fails, exit the process

        default: // parent process, pid now contains the child pid
            close (pipe_fd[1]);

            {
                FILE* stream = fdopen (pipe_fd[0], "r");
                char* line;
                size_t len;
                ssize_t read;
                do {
                    line = NULL;
                    read = getline (&line, &len, stream);
                    if (len != 0) {
                        ret_vec.emplace_back (line);
                    }
                    free (line);
                } while (read != -1);
            }
            close (pipe_fd[0]);
    }


    return;
}
