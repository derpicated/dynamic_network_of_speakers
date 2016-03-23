#include <iostream>
#include <unistd.h>

#include "../client/audio.hpp"

int main (int argc, char const* argv[]) {
   if(argc != 2){
      std::cout << "usage: audio_test file.mp3" << std::endl;
      return 1;//throw();
   }

   //TODO use commandline argument1 as input file
   audio_player test_player("/home/Johannux/Music/around_the_world.mp3");

   test_player.play();

   sleep(5);

   test_player.stop();

   sleep(1);

   test_player.play(4);

   sleep(5);

   test_player.set_volume(10);

   sleep(5);

   test_player.set_volume(50);

   sleep(5);

   test_player.set_volume(1);

   sleep(5);

   test_player.stop();

   return 0;
}
