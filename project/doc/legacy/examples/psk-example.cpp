#include <iostream>
#include <string>

#include "mwav.h"

int main() {
  std::string message = "Hello World!\n";
  std::string callsign =
      "TSTCLL";  // Optional, do not pass anything if not needed.

  mwav::EncodeString(mwav::DataModulation::BPSK_125, message, "psk-example.wav",
                     callsign);
}