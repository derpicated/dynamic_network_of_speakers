#include <iostream>
#include <mosquittopp.h>
#include <signal.h>
#include <sys/stat.h>

#include "./libs/config/config_parser.hpp"
#include "./libs/logger/easylogging++.h"
#include "dns.hpp"

INITIALIZE_EASYLOGGINGPP

using namespace std;

volatile bool receivedSIGINT{ false };

void handleSIGINT (int) {
    receivedSIGINT = true;
}

void handleSIGCHLD (int) {
    /*TODO implement SIGCHLD handler*/
    int status;
    wait (&status); // pid_t pid =
    /*lookup pid in map to find function
    remove pid entry from map
    call function
    */
}

int main (int argc, char const* argv[]) {
    signal (SIGINT, handleSIGINT);
    signal (SIGCHLD, handleSIGCHLD);

    int mosquitto_lib_version[] = { 0, 0, 0 };

    if (argc != 2) {
        LOG (FATAL) << "usage: " << argv[0] << " config.js" << std::endl;
        exit (EXIT_FAILURE);
    }
    config_parser CONFIG (argv[1]);

    LOG (INFO) << CONFIG.project_name () << " v" << CONFIG.version () << std::endl;

    mosqpp::lib_init ();
    mosqpp::lib_version (&mosquitto_lib_version[0], &mosquitto_lib_version[1],
    &mosquitto_lib_version[2]);
    LOG (DEBUG) << "using Mosquitto lib version " << mosquitto_lib_version[0] << '.'
                << mosquitto_lib_version[1] << '.' << mosquitto_lib_version[2];
    try {
        dns client (CONFIG);

        while (!receivedSIGINT) {
            int rc = client.loop ();
            if (rc) {
                LOG (ERROR) << "MQTT: attempting reconnect";
                client.reconnect ();
            }
        }
        LOG (FATAL) << "Revieced signal for signalhandler";
    } catch (exception& e) {
        LOG (FATAL) << "Exception " << e.what ();
    } catch (...) {
        LOG (FATAL) << "UNKNOWN EXCEPTION";
    }

    LOG (INFO) << CONFIG.project_name () << " stopped";
    mosqpp::lib_cleanup ();

    return 0;
}
