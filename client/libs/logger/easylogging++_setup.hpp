#ifndef EASYLOGGINGPP_SETUP_HPP
#define EASYLOGGINGPP_SETUP_HPP

#include "./easylogging++.h"

// logger defines and configuration
#define ELPP_NO_DEFAULT_LOG_FILE
INITIALIZE_EASYLOGGINGPP

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

#endif // EASYLOGGINGPP_SETUP_HPP
