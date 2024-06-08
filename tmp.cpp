#include <SignalEasel/ax25.hpp>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace signal_easel;

int main() {
  ax25::Frame frame;
  ax25::Address src_address("NOCALL", 1);
  ax25::Address dst_address("APRS", 0);
  ax25::Address repeater_address("WIDE1", 1);
  frame.setDestinationAddress(dst_address);
  frame.setSourceAddress(src_address);
  frame.addRepeaterAddress(repeater_address);

  std::string info = "@092345z/:*E\";qZ=OMRC/A=088132Hello World!";
  std::vector<uint8_t> data(info.begin(), info.end());
  frame.setInformation(data);

  std::cout << frame << "\n";
  auto out_frame = frame.encodeFrame();

  for (auto c : out_frame) {
    std::cout << "0x";
    if (c < 0x10) {
      std::cout << "0";
    }
    std::cout << std::hex << (unsigned int)c << ", ";
  }

  std::cout << std::endl;

  return 0;
}