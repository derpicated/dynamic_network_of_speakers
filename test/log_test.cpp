#include <iostream>
#include <string>

#include "../client/libs/logger/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main (int argc, char const* argv[]) {
    LOG (WARNING) << "My first info log using default logger";
    el::Loggers::addFlag (el::LoggingFlag::ColoredTerminalOutput);
    LOG (ERROR) << "My first info log using default logger";

    return 0;
}
