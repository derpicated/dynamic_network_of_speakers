#include <SFML/Audio.hpp>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

int main (int argc, char const* argv[]) {
    if (argc != 2) {
        std::cout << "usage: audio_test file.ogg" << std::endl;
        return 1; // throw();
    }

    std::string test_file = argv[1];

    struct stat buffer;
    if (stat (test_file.c_str (), &buffer) != 0) {
        std::perror ("error, file:");
    }

    sf::Music test_music;

    test_music.openFromFile (test_file);

    test_music.play ();

    sleep (5);

    test_music.stop ();

    sleep (1);

    test_music.play ();

    sleep (5);

    test_music.pause ();

    sleep (5);

    test_music.play ();

    sleep (5);

    test_music.setVolume (30);

    sleep (5);

    test_music.setVolume (75);

    sleep (5);

    test_music.setVolume (1);

    sleep (5);

    test_music.setVolume (50);
    test_music.stop ();

    return 0;
}
