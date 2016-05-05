#include "Config.h"
#include <random>
#include <unistd.h>

const char* getClientID () {
    std::string clientid{ "speaker_" };
    std::random_device rd;
    int result;
    std::uniform_int_distribution<int> distribution (0, 999);
    result = distribution (rd);

    if (result < 100) {
        return clientid.append ("0").append (std::to_string (result)).c_str ();
    } else {
        return clientid.append (std::to_string (result)).c_str ();
    }
}
