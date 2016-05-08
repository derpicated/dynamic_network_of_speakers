#include "config_parser.hpp"

config_parser::broker_type::broker_type ()
: name ("default_Mosquitto")
, uri ("test.mosquitto.org")
, port (1883)
, port_encrypted (8883)
, port_encrypted_certificate (8884)
, port_ws (8080)
, port_ws_encrypted (8081){};

config_parser::config_parser (std::string config_file)
: config_file_location (config_file)
, config_string{}
, _parser{} {
    load_config_file ();
}

config_parser::~config_parser () {
}

void config_parser::load_config_file () {
    std::string line;
    std::string data;
    std::ifstream config_file;
    std::string delete_from_file[] = { "var CONFIG =", ";" };
    try {
        config_file.open (config_file_location);
        while (getline (config_file, line)) {
            data.append (line);
        }
        for (auto elem : delete_from_file) {
            data.replace (data.find (elem), elem.length (), "");
        }
        config_file.close ();
    } catch (std::ifstream::failure e) {
        std::cerr << "Exception opening/reading/closing config file" << std::endl;
        exit (EXIT_FAILURE);
    } catch (std::out_of_range) {
        std::cerr << "Exception in config file" << std::endl;
        exit (EXIT_FAILURE);
    } catch (...) {
        std::cerr << "Undefined Exception in config parser" << std::endl;
        exit (EXIT_FAILURE);
    }
    config_string = data;
}

void config_parser::print_config_string () {
    std::cout << config_string << std::endl;
}

std::string config_parser::project_name (bool fullname) {
    Jzon::Parser parser;
    Jzon::Node root_node = parser.parseString (config_string);
    return fullname ? root_node.get ("name").toString () :
                      root_node.get ("name_short").toString ();
}

std::string config_parser::version () {
    Jzon::Parser parser;
    Jzon::Node root_node = parser.parseString (config_string);
    int version_major    = root_node.get ("version").get ("major").toInt ();
    int version_minor    = root_node.get ("version").get ("minor").toInt ();
    int version_revision = root_node.get ("version").get ("revision").toInt ();
    return std::to_string (version_major) + "." +
    std::to_string (version_minor) + "." + std::to_string (version_revision);
}

std::string config_parser::speaker_prefix () {
    Jzon::Parser parser;
    Jzon::Node root_node = parser.parseString (config_string);
    return root_node.get ("name_speaker").toString ();
}

std::string config_parser::site_prefix () {
    Jzon::Parser parser;
    Jzon::Node root_node = parser.parseString (config_string);
    return root_node.get ("name_website").toString ();
}

config_parser::broker_type config_parser::broker () {
    Jzon::Parser parser;
    Jzon::Node root_node = parser.parseString (config_string);

    broker_type broker_used;
    std::string broker_select = std::to_string (broker_selector ());
    broker_used.name =
    root_node.get ("broker").get (broker_select).get ("broker").toString ();
    broker_used.uri  = root_node.get ("broker").get (broker_select).get ("uri").toString ();
    broker_used.port = root_node.get ("broker").get (broker_select).get ("port").toInt ();
    broker_used.port_encrypted =
    root_node.get ("broker").get (broker_select).get ("port_encrypted").toInt ();
    broker_used.port_encrypted_certificate = root_node.get ("broker")
                                             .get (broker_select)
                                             .get ("port_encrypted_certificate")
                                             .toInt ();
    broker_used.port_ws =
    root_node.get ("broker").get (broker_select).get ("port_ws").toInt ();
    broker_used.port_ws_encrypted =
    root_node.get ("broker").get (broker_select).get ("port_ws_encrypted").toInt ();

    return broker_used;
}

int config_parser::broker_selector () {
    Jzon::Parser parser;
    Jzon::Node root_node = parser.parseString (config_string);
    return root_node.get ("use_broker").toInt ();
}
