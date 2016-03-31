#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "../client/libs/download/download.hpp"

int main (int argc, char const* argv[]) {
    if (argc != 3) {
        std::cout << "usage: download_test remote local" << std::endl;
        return 1; // throw();
    }

    std::string test_url  = argv[1];
    std::string test_file = argv[2];
    struct stat buffer;
    if (stat (test_file.c_str (), &buffer) == 0) {
        std::perror ("error, file exists");
    }

    download::download (test_url, test_file);

    return 0;
}
