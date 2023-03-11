
#include <iostream>
#include <bitset>
#include <iomanip>

#include "ax25.h"
#include "modulators.h"
#include "wavgen.h"

int main() {
  std::cout << std::bitset<8>(0x55) << std::endl;
  std::cout << std::bitset<8>(AX25::reverse_bits(0x55)) << std::endl;


  AX25::Address address1("KK7EWJ", 0, false);
  AX25::Address address2("KD9GDC", 11, true);
  std::vector<uint8_t> information = {};

  std::string message = "Hello World!";
  for (unsigned int i = 0; i < message.length(); i++) {
    information.push_back(message[i]);
  }

  AX25::Frame frame;
  frame.AddAddress(address1);
  frame.AddAddress(address2);
  frame.AddInformation(information);
  frame.BuildFrame();
  frame.Print(true);
  frame.PrintBitStream(true);

  WavGen wavgen("ax25-test.wav");
  modulators::AfskBitStream(wavgen, frame.GetBitStream());
  wavgen.done();
  return 0;
}