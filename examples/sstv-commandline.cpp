#include "mwav.h"

#include <string>
#include <cstdio>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cout << "arg error: sstv-gen input.png output.wav callsign";
    return 1;
  }
  
  std::string input_image = argv[1];
  std::string output_wav = argv[2];
  std::string callsign = argv[3];


  mwav::EncodeSSTV(output_wav, input_image, false, callsign);
}