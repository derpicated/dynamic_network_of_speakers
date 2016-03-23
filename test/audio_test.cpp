#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

#include "../client/audio.hpp"

int main (int argc, char const* argv[]) {
   if(argc != 2){
      std::cout << "usage: audio_test file.mp3" << std::endl;
      return 1;//throw();
   }

   std::string test_file = argv[1];

   struct stat buffer;
   if(stat (test_file.c_str(), &buffer) != 0){
       std::perror("error, file:");
   }

   audio_player test_player(test_file);

   test_player.play();

   sleep(5);

   test_player.stop();

   sleep(1);

   test_player.play(4);

   sleep(5);

   test_player.set_volume(30);

   sleep(5);

   test_player.set_volume(75);

   sleep(5);

   test_player.set_volume(1);

   sleep(5);

   test_player.set_volume(50);
   test_player.stop();

   return 0;
}
