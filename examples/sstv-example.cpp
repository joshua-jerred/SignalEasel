#include <iostream>
#include "mwav.h"

int main()
{
    std::string output_file_path = "sstv-output.wav";
    std::string input_image_path = "test4.png";
    std::string callsign = "APOLLO";
    mwav::EncodeSSTV(output_file_path, input_image_path, true, callsign);
    return 0;
}