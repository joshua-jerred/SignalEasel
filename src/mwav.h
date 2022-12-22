#ifndef MWAV_H_
#define MWAV_H_

#include <string>

#include "psk.h"

namespace MWAVData {

    enum class MODE {
        ASCII = 0,
        RAW_BIN = 1,
    };

    enum class MODULATION {
        BPSK_125,
        BPSK_250,
        BPSK_500,
        BPSK_1000,
        QPSK_125,
        QPSK_250,
        QPSK_500,
        AFSK1200
    };

    bool encode(MODE mode, 
                MODULATION modulation, 
                std::string input, 
                std::string out_file_path);
}

#endif // MWAV_H_