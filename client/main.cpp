#include <iostream>
#include <mosquittopp.h>
#include <signal.h>
#include <sys/stat.h>

#include "AppInfo.h"
#include "Config.h"
#include "DNSMusic.h"

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

int main (void /*int argc, char* argv[]*/) {
    signal (SIGINT, handleSIGINT);
    signal (SIGCHLD, handleSIGCHLD);

    int major{ 0 };
    int minor{ 0 };
    int revision{ 0 };

    // temp test
    /*if (argc != 1) {
        std::cout << "usage: " << argv[0] << " file.ogg" << std::endl;
        return 1;
    }

    std::string music_file = argv[1];

    struct stat buffer;
    if (stat (music_file.c_str (), &buffer) != 0) {
        std::perror ("error, file");
        return 1;
    }*/
    // end temp

    cout << "-- MQTT application: " << APPNAME_VERSION << "  ";
    mosqpp::lib_init ();
    mosqpp::lib_version (&major, &minor, &revision);
    cout << "uses Mosquitto lib version " << major << '.' << minor << '.'
         << revision << endl;
    try {
        DNSMusic client ("DNSMusic", "tempDNS", getClientID ());

        // auto clients{ static_cast<mosqpp::mosquittopp*> (&tempDNS) };

        while (!receivedSIGINT) {
            // for (auto client : clients) {
            int rc = client.loop ();
            if (rc) {
                cerr << "-- MQTT reconnect" << std::endl;
                client.reconnect ();
            }
            //}
        }
    } catch (exception& e) {
        cerr << "Exception " << e.what () << std::endl;
    } catch (...) {
        cerr << "UNKNOWN EXCEPTION \n";
    }

    cout << "-- MQTT application: " << APPNAME_VERSION << " stopped" << endl
         << endl;
    mosqpp::lib_cleanup ();

    return 0;
}
