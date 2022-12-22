#ifndef MODULATORS_H_
#define MODULATORS_H_

#include "wavgen.h"

namespace MODULATE {
    bool addMorseCallsign(WavGen &wavgen, std::string callsign);
    bool addPskASCII(WavGen &wavgen, std::string message);
}

#endif // MODULATORS_H_