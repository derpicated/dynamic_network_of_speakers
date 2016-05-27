#ifndef DOWNLOAD_HPP
#define DOWNLOAD_HPP

#include <SFML/Network/Http.hpp>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <wait.h>

#include "../logger/easylogging++.h"

namespace download {
void download (std::string remote_uri, std::string local_name);
};

#endif // DOWNLOAD_HPP
