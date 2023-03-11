#ifndef MODULATORS_H_
#define MODULATORS_H_

#include <vector>

#include "bit-stream.h"
#include "wavgen.h"
#include "mwav.h"

namespace modulators {
using mwav::DataModulation;
bool PskEncodeBinary(WavGen &wavgen, const std::vector<uint8_t> &message,
                  const mwav::DataModulation &modulation_type);
                  
bool PskEncodeAscii(WavGen &wavgen, const std::string &message,
                 const mwav::DataModulation &modulation_type);

bool AfskEncodeAscii(WavGen &wavgen, const std::string &message);
bool AfskEncodeBinary(WavGen &wavgen, const std::vector<uint8_t> &data);
bool AfskEncodeBitStream(WavGen &wavgen, BitStream &bit_stream);

bool AprsEncodePacket(WavGen &wavgen,
                  const mwav::AprsRequiredFields &required_fields,
                  const mwav::aprs_packet::AprsLocationData &location_data = {},
                  const mwav::AprsTelemetryData &telemetry_data = {});
                  
bool EncodeMorse(WavGen &wavgen, std::string callsign);
}


#endif  // MODULATORS_H_