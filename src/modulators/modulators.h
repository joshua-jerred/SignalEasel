#ifndef MODULATORS_H_
#define MODULATORS_H_

#include <vector>

#include "wavgen.h"
#include "aprs.h"

namespace modulators {
    
enum class PskMode { BPSK, QPSK };

enum class PskSymbolRate {
  S125,
  S250,
  S500,
  S1000  // BPSK only
};



bool Morse(WavGen &wavgen, std::string callsign);

bool PskAscii(WavGen &wavgen, const std::string &message, const PskMode &mode,
              const PskSymbolRate &sym_rate);

bool AfskAscii(WavGen &wavgen, const std::string &message);

bool PskBinary(WavGen &wavgen, const std::vector<uint8_t> &message,
               const PskMode &mode, const PskSymbolRate &sym_rate);

bool AfskBinary(WavGen &wavgen, const std::vector<uint8_t> &data);

bool APRSLocation(WavGen &wavgen, const Aprs::Location &location);
}  // namespace modulators

#endif  // MODULATORS_H_