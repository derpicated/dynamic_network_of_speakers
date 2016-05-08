#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdexcept>

#include "libs/jzon/Jzon.h"

class config_parser {
    public:
    class broker_type {
        public:
        broker_type ();
        std::string name;
        std::string uri;
        int port;
        int port_encrypted;
        int port_encrypted_certificate;
        int port_ws;
        int port_ws_encrypted;
    };
    config_parser (std::string config_file);
    ~config_parser ();
    void load_config_file ();
    std::string project_name (bool fullname = true);
    std::string version ();
    std::string speaker_prefix ();
    std::string site_prefix ();
    broker_type broker ();
    int broker_selector ();
    void print_config_string ();

    protected:
    private:
    const std::string config_file_location_default{ "./config/config.js" };
    std::string config_file_location;
    std::string config_string;
    Jzon::Parser _parser;
};

#endif
