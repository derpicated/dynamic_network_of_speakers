#include <iostream>
#include <mosquittopp.h>
#include <signal.h>
#include <sys/stat.h>

#include "./libs/config/config_parser.hpp"
#include "./libs/logger/easylogging++.h"
#include "dns.hpp"

// logger defines and configuration
#define ELPP_NO_DEFAULT_LOG_FILE
INITIALIZE_EASYLOGGINGPP

// declarations
volatile bool receivedSIGINT{ false };
void handleSIGCHLD (int);
void handleSIGINT (int);
void configure_logger (std::string log_level, std::string log_file);

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

// logger configuration
void configure_logger (std::string log_level_str, std::string log_file_str) {
    el::Configurations log_conf;

    if (log_file_str.empty ()) {
        log_conf.set (
        el::Level::Global, el::ConfigurationType::ToFile, "false");
    } else {
        log_conf.set (el::Level::Global, el::ConfigurationType::Filename,
        log_file_str.c_str ());
    }


    el::Level log_level = el::LevelHelper::convertFromString (log_level_str.c_str ());
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
            LOG (ERROR) << "config: false log level: \"" << log_level_str << "\"";
    }
    el::Loggers::reconfigureAllLoggers (
    el::ConfigurationType::Format, "%datetime %level : %msg");
    log_conf.set (el::Level::Debug, el::ConfigurationType::Format,
    "%datetime %level: at %loc : %msg");

    el::Loggers::reconfigureAllLoggers (log_conf);
    el::Loggers::addFlag (el::LoggingFlag::DisableApplicationAbortOnFatalLog);
    el::Loggers::addFlag (el::LoggingFlag::ColoredTerminalOutput);
}
