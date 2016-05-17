#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <random>

#include "../jzon/Jzon.h"
#include "../logger/easylogging++.h"

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
    std::string clientid ();
    std::string log_level ();
    std::string log_file ();
    void set_client_id (std::string clientid);
    broker_type broker ();
    int broker_selector ();
    void print_config_string ();
    std::string topic (std::string topic_name);

    protected:
    private:
    std::string generate_name (std::string prefix, int min = 0, int max = 999);
    std::string config_file_location;
    std::string config_string;
    std::string _clientid;
    Jzon::Parser _parser;
};

#endif
