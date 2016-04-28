#include "../client/Config.h"
#include "../client/DNSDataParser.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

void test_prarse_client_data ();
void test_prarse_source_data ();

int main () {
    test_prarse_client_data ();
    std::cout << "Passed client data parsing asserts" << std::endl;
    test_prarse_source_data ();
    std::cout << "Passed client data parsing asserts" << std::endl;
    std::cout << "Passed all asserts" << std::endl;

    return 0;
}

void test_prarse_client_data () {
    dataParser dp;
    std::ifstream client_data_file ("test_client_data.json");
    if (!client_data_file.is_open ()) {
        std::cout << "Unable to open file test_client_data.json" << std::endl;
    }

    // read file into string
    std::stringstream buffer;
    buffer << client_data_file.rdbuf ();
    std::string json_str = buffer.str ();

    // replace client id
    std::string search_str = "clientid2";
    int pos = json_str.find (search_str);
    if (pos != (-1)) {
        json_str.replace (pos, search_str.length (), CLIENT_XXX);
    } else {
        std::cout << "Defective file test_client_data.json, altered string" << std::endl;
    }

    // parse json string
    std::map<std::string, std::vector<float>> objects;
    speakerData local_speaker = dp.parseClientData (json_str, objects);

    // check the returned data
    assert (local_speaker.speakerid == CLIENT_XXX);
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
    dataParser dp;
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
    assert (
    sources["fsf_song"] == "http://www.gnu.org/music/free-software-song.ogg");
    assert (sources["song_n"] == "http://www.dns.net/test.ogg");
}
