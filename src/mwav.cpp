#include "mwav.h"

bool MWAVData::encode( // <-- Does not include a morse code callsign
            MWAVData::MODE mode, 
            MODULATION modulation, 
            std::string input, 
            std::string out_file_path
            ) {
    WavGen wavgen = WavGen(out_file_path);
    return true;
}

bool MWAVData::encode(// <-- Includes a morse code callsign
            MWAVData::MODE mode,
            MWAVData::MODULATION modulation, 
            std::string input, 
            std::string out_file_path,
            std::string callsign
            ) {
    WavGen wavgen = WavGen(out_file_path);

    if (!modulators::Morse(wavgen, callsign)) {// Add the callsign
        return false;
    };

    wavgen.addSineWave(0, 0, 0.5); // Add a 0.5 second pause
    
    if (!modulators::PskAscii(wavgen, input, modulators::PskMode::BPSK, modulators::PskSymbolRate::S125)) { // Add the PSK ASCII data
        return false;
    }; 

    return true;
}