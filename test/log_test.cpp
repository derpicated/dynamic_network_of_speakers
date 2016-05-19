#include <iostream>
#include <string>

#include "../client/libs/logger/easylogging++.h"

#define ELPP_NO_DEFAULT_LOG_FILE
INITIALIZE_EASYLOGGINGPP

int main () {
    LOG (DEBUG) << "unconfigured DEBUG log";
    LOG (INFO) << "unconfigured INFO log";
    LOG (WARNING) << "unconfigured WARNING log";
    LOG (ERROR) << "unconfigured ERROR log";
    // LOG (FATAL) << "unconfigured FATAL log"; //will end the program


    std::string log_lvl_str = "INFO";
    std::string log_file    = "";


    el::Configurations log_conf;
    if (log_file.empty ()) {
        log_conf.set (
        el::Level::Global, el::ConfigurationType::ToFile, "false");
    } else {
        log_conf.set (
        el::Level::Global, el::ConfigurationType::Filename, log_file.c_str ());
    }


    el::Level log_level = el::LevelHelper::convertFromString (log_lvl_str.c_str ());
    switch (log_level) {
        case el::Level::Fatal:
            log_conf.set (
            el::Level::Error, el::ConfigurationType::Enabled, "false");
        case el::Level::Error:
            log_conf.set (el::Level::Warning, el::ConfigurationType::Enabled, "false");
        case el::Level::Warning:
            log_conf.set (
            el::Level::Info, el::ConfigurationType::Enabled, "false");
        case el::Level::Info:
            log_conf.set (
            el::Level::Debug, el::ConfigurationType::Enabled, "false");
        case el::Level::Debug: break;
        case el::Level::Global:
        case el::Level::Trace:
        case el::Level::Unknown:
        case el::Level::Verbose:
        default:
            LOG (ERROR) << "CONFIG: false log level: \"" << log_lvl_str << "\"";
    }

    el::Loggers::reconfigureLogger ("default", log_conf);

    el::Loggers::addFlag (el::LoggingFlag::DisableApplicationAbortOnFatalLog);
    el::Loggers::addFlag (el::LoggingFlag::ColoredTerminalOutput);

    LOG (DEBUG) << "configured DEBUG log";
    LOG (INFO) << "configured INFO log";
    LOG (WARNING) << "configured WARNING log";
    LOG (ERROR) << "configured ERROR log";
    LOG (FATAL) << "configured FATAL log";
    return 0;
}
