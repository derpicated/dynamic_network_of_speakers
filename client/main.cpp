#include <iostream>
#include <mosquittopp.h>
#include <signal.h>
#include <sys/stat.h>

#include "./libs/config/config_parser.hpp"
#include "dns.hpp"

using namespace std;

volatile bool receivedSIGINT{ false };

void handleSIGINT (int) {
    cerr << "revieced signal for signalhandler" << endl;
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
        std::cout << "usage: " << argv[0] << " config.js" << std::endl;
        exit (EXIT_FAILURE);
    }
    config_parser CONFIG (argv[1]);

    std::cout << CONFIG.project_name () << " v" << CONFIG.version () << std::endl;

    mosqpp::lib_init ();
    mosqpp::lib_version (&mosquitto_lib_version[0], &mosquitto_lib_version[1],
    &mosquitto_lib_version[2]);
    cout << "uses Mosquitto lib version " << mosquitto_lib_version[0] << '.'
         << mosquitto_lib_version[1] << '.' << mosquitto_lib_version[2] << endl;
    try {
        dns client (CONFIG);

        while (!receivedSIGINT) {
            int rc = client.loop ();
            if (rc) {
                cerr << "-- MQTT reconnect" << std::endl;
                client.reconnect ();
            }
        }
    } catch (exception& e) {
        cerr << "Exception " << e.what () << std::endl;
    } catch (...) {
        cerr << "UNKNOWN EXCEPTION \n";
    }

    cout << "-- MQTT application: " << CONFIG.project_name () << " stopped" << endl
         << endl;
    mosqpp::lib_cleanup ();

    return 0;
}
