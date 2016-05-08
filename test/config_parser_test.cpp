#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "../client/libs/config/config_parser.hpp"

int main (int argc, char const* argv[]) {
    if (argc != 2) {
        std::cout << "usage: config file" << std::endl;
        exit (EXIT_FAILURE);
    }
    config_parser test_config (argv[1]);

    std::cout << "Project name: " << test_config.project_name () << std::endl;
    std::cout << "Project name short: " << test_config.project_name (false) << std::endl;
    std::cout << "v" << test_config.version () << std::endl;
    std::cout << "Speaker prefix: " << test_config.speaker_prefix () << std::endl;
    std::cout << "Client ID: " << test_config.clientid () << std::endl;
    std::cout << "Website prefix: " << test_config.site_prefix () << std::endl;

    std::cout << "Using broker: " << test_config.broker_selector () << std::endl;

    std::cout << "Name: " << test_config.broker ().name << std::endl;
    std::cout << "URI: " << test_config.broker ().uri << std::endl;
    std::cout << "Port: " << test_config.broker ().port << std::endl;
    std::cout << "Port encrypted: " << test_config.broker ().port_encrypted << std::endl;
    std::cout << "Port encrypted certificate: " << test_config.broker ().port_encrypted_certificate
              << std::endl;
    std::cout << "Port ws: " << test_config.broker ().port_ws << std::endl;
    std::cout << "Port ws encrypted: " << test_config.broker ().port_ws_encrypted
              << std::endl;
    return 0;
}
