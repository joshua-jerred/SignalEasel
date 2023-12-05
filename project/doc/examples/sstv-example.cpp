#include <iostream>

#include "mwav.h"

int main() {
  std::string output_file_path = "sstv-output.wav";
  std::string input_image_path = "test1.png";
  std::string callsign = "NOCALL";

  std::vector<std::string> data = {"This", "is a", "test"};
  
  mwav::EncodeSSTV(output_file_path, input_image_path, true, callsign,
                   mwav::Sstv_Mode::ROBOT_36, data);
  return 0;
}