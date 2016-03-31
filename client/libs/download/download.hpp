#ifndef DOWNLOAD_HPP
#define DOWNLOAD_HPP

#include <signal.h>
#include <string>
#include <unistd.h>
#include <wait.h>

namespace download {
void download (std::string remote_uri, std::string locale_name);
};

#endif // DOWNLOAD_HPP
