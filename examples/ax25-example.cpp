
#include <iostream>
#include <bitset>
#include <iomanip>

#include "ax25.h"
#include "modulators.h"
#include "wavgen.h"

int main() {
  AX25::Address address1("APRS", 0, false);
  AX25::Address address2("TSTCAL", 11, true);
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

  WavGen wavgen("ax25-example.wav");
  modulators::AfskEncodeBitStream(wavgen, frame.GetBitStream());
  wavgen.done();
  return 0;
}