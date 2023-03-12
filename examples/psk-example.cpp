#include <iostream>
#include <string>

#include "mwav.h"

int main() {
  std::string message = "Hello World!";
  if (!mwav::EncodeString(mwav::DataModulation::BPSK_125, message,
                          "psk-example.wav", "TESTCL")) {
    std::cout << "Failed to encode message." << std::endl;
  } else {
    std::cout << "Encoded message and saved to psk-example.wav" << std::endl;
  }
}