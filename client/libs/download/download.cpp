#include "download.hpp"

void download::download (std::string remote_uri, std::string locale_name) {
    pid_t child_pid = fork ();
    switch (child_pid) {
        case -1: // error
            std::perror ("failed to start audio player");
            break;

        case 0: // child process
            execlp ("wget", "wget", "-q", "-O", locale_name.c_str (),
            remote_uri.c_str (), (char*)NULL);
            std::perror ("error executing wget");
            exit (1);

        default: // parent process, pid now contains the child pid
                 ;
    }
}
