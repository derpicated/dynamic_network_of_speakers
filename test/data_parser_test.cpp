#include "../client/data_parser.hpp"
#include "../client/libs/logger/easylogging++_setup.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

void test_prarse_client_data ();
void test_prarse_source_data ();
void test_compose_client_data ();

int main () {
    configure_logger ("INFO", "./logs/data_parser_test.log");
    test_prarse_client_data ();
    std::cout << "Passed client data parsing asserts" << std::endl;
    test_prarse_source_data ();
    std::cout << "Passed client data parsing asserts" << std::endl;
    test_compose_client_data ();
    std::cout << "Passed client data composing asserts" << std::endl;
    std::cout << "Passed all asserts" << std::endl;

    return 0;
}

void test_prarse_client_data () {
    data_parser dp;
    std::ifstream client_data_file ("test_client_data.json");
    if (!client_data_file.is_open ()) {
        std::cout << "Unable to open file test_client_data.json" << std::endl;
    }

    // read file into string
    std::stringstream buffer;
    buffer << client_data_file.rdbuf ();
    std::string json_str = buffer.str ();

    // parse json string
    std::map<std::string, std::vector<float>> objects;
    speakerData local_speaker = dp.parseClientData (json_str, "clientid2", objects);

    // check the returned data
    assert (local_speaker.speakerid == "clientid2");
    assert (local_speaker.objects.size () == 2);
    assert (local_speaker.objects["objectid1"].distance == 1);
    assert (local_speaker.objects["objectid1"].angle == 90);
    assert (local_speaker.objects["objectid2"].distance == 42);
    assert (3.14 < local_speaker.objects["objectid2"].angle &&
    local_speaker.objects["objectid2"].angle < 3.16); // NEVER USE IN PRODUCTION

    assert (objects.size () == 2);
    assert (objects["objectid1"].size () == 2);
    assert (objects["objectid1"].at (0) == 1);
    assert (objects["objectid1"].at (1) == 5);

    assert (objects["objectid2"].size () == 2);
    assert (objects["objectid2"].at (0) == 3);
    assert (objects["objectid2"].at (1) == 7);
}

void test_prarse_source_data () {
    data_parser dp;
    std::ifstream source_data_file ("test_source_data.json");
    if (!source_data_file.is_open ()) {
        std::cout << "Unable to open file test_source_data.json" << std::endl;
    }

    // read file into string
    std::stringstream buffer;
    buffer << source_data_file.rdbuf ();
    std::string json_str = buffer.str ();

    // parse json string
    std::map<std::string, std::string> sources;
    sources = dp.parseAudioSourceData (json_str);

    assert (sources.size () == 4);
    assert (sources["song_1"] == "http://www.hello.internet/fm/song.ogg");
    assert (sources["song_2"] == "http://www.website.com/music.ogg");
    assert (sources["fsf_song.ogg"] ==
    "http://www.gnu.org/music/free-software-song.ogg");
    assert (sources["song_n.nth"] == "http://www.dns.net/test.ogg");
}

void test_compose_client_data () {
    data_parser dp;
    std::ifstream source_data_file ("test_client_compose.json");
    if (!source_data_file.is_open ()) {
        std::cout << "Unable to open file test_client_compose.json" << std::endl;
    }

    // read file into string
    std::stringstream buffer;
    buffer << source_data_file.rdbuf ();
    std::string json_str = buffer.str ();

    // set  local speaker
    speakerData local_speaker;
    audioObject object1, object2;
    local_speaker.speakerid            = "clientid2";
    object1.distance                   = 1;
    object1.angle                      = 90;
    local_speaker.objects["objectid1"] = object1;
    object2.distance                   = 42;
    object2.angle                      = 3.15;
    local_speaker.objects["objectid2"] = object2;

    // compose json string
    std::string composed_str = dp.composeClientData (local_speaker);

    // check the returned data
    composed_str += '\n';
    assert (composed_str == json_str);
}
