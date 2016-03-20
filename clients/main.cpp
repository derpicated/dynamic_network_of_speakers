#include "AppInfo.h"
#include "Config.h"
#include "DNSMusic.h"

#include <signal.h>
#include <iostream>

using namespace std;

volatile bool receivedSIGINT {false};

void handleSIGINT(int )
{
  cerr << "revieced signal for signalhandler" << endl;
  receivedSIGINT = true;
}

int main(int argc, char *argv[])
{
  signal(SIGINT, handleSIGINT);

  int major {0};
  int minor {0};
  int revision {0};

  cout << "-- MQTT application: " << APPNAME_VERSION << "  ";
  mosqpp::lib_init();
  mosqpp::lib_version(&major, &minor, &revision);
  cout << "uses Mosquitto lib version "
       << major << '.' << minor << '.' << revision << endl;
  try
  {
    //TemperatureConverter tempconv("fccf", "tempconv", MQTT_BROKER, 1883);
    DNSMusic  tempDNS("DNSMusic", "tempDNS", MQTT_BROKER, 1883);
    auto clients {static_cast<mosqpp::mosquittopp*>(&tempDNS)};

    while (!receivedSIGINT)
    {
      for (auto client: clients)
      {
        int rc = client->loop();
        if (rc)
        {
          cerr << "-- MQTT reconnect" << std::endl;
          client->reconnect();
        }
      }

    }
  }
  catch(exception& e)
  {
    cerr << "Exception " << e.what() << std::endl;
  }
  catch(...)
  {
    cerr << "UNKNOWN EXCEPTION \n";
  }

  cout << "-- MQTT application: " << APPNAME_VERSION << " stopped"
       << endl << endl;
  mosqpp::lib_cleanup();

  return 0;
}
