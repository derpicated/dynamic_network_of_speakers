#include <iostream>
#include <mosquittopp.h>
#include <signal.h>
#include <sys/stat.h>

#include "./libs/config/config_parser.hpp"
#include "./libs/logger/easylogging++_setup.hpp"
#include "dns.hpp"

// declarations
volatile bool receivedSIGINT{ false };
void handleSIGCHLD (int);
void handleSIGINT (int);

// main
int main (int argc, char const* argv[]) {
    // if no configuration was given, exit
    if (argc != 2) {
        LOG (FATAL) << "usage: " << argv[0] << " config.js";
        exit (EXIT_FAILURE);
    }

    // set signal handlers
    signal (SIGINT, handleSIGINT);
    signal (SIGCHLD, handleSIGCHLD);

    // create configuration from file
    config_parser config (argv[1]);
    LOG (INFO) << config.project_name () << " v" << config.version ();
    configure_logger (config.log_level (), config.log_file ());

    // init libmosquitto
    int mosquitto_lib_version[] = { 0, 0, 0 };
    mosqpp::lib_init ();
    mosqpp::lib_version (&mosquitto_lib_version[0], &mosquitto_lib_version[1],
    &mosquitto_lib_version[2]);
    LOG (DEBUG) << "using Mosquitto lib version " << mosquitto_lib_version[0] << '.'
                << mosquitto_lib_version[1] << '.' << mosquitto_lib_version[2];

    // run dns
    try {
        dns client (config);

        while (!receivedSIGINT) {
            int rc = client.loop ();
            if (rc) {
                LOG (ERROR) << "MQTT: attempting reconnect";
                client.reconnect ();
            }
        }
        LOG (FATAL) << "Revieced signal for signalhandler";
    } catch (std::exception& e) {
        LOG (FATAL) << "Exception " << e.what ();
    } catch (...) {
        LOG (FATAL) << "UNKNOWN EXCEPTION";
    }

    LOG (INFO) << config.project_name () << " stopped";
    mosqpp::lib_cleanup ();

    return 0;
}

// signal handlers
void handleSIGINT (int) {
    receivedSIGINT = true;
}

void handleSIGCHLD (int) {
    /*TODO implement SIGCHLD handler*/
    int status;
    wait (&status);
}
