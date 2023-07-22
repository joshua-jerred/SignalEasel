#ifndef MODULATORS_H_
#define MODULATORS_H_

#include <vector>

#include "bit-stream.h"
#include "data_modulation.hpp"
#include "wav_gen.hpp"

namespace modulators {
// bool PskEncodeBinary(WavGen &wavgen, const std::vector<uint8_t> &message,
//                      const mwav::DataModulation &modulation_type);

bool pskEncodeAscii(wavgen::Generator &wavgen, const std::string &message,
                    mwav::data::Mode modulation_type);

bool afskEncodeAscii(wavgen::Generator &wavgen, const std::string &message);

// bool AfskEncodeBinary(WavGen &wavgen, const std::vector<uint8_t> &data);

// bool AfskEncodeBitStream(WavGen &wavgen, BitStream &bit_stream);

// bool EncodeMorse(WavGen &wavgen, std::string callsign);

// bool AprsEncodePositionPacket(
//     WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
//     const mwav::aprs_packet::CompressedPositionReport &location_data);
// bool AprsEncodeTelemetryData(
//     WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
//     const mwav::aprs_packet::Telemetry &telemetry_data);
// bool AprsEncodeTelemetryParamName(
//     WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
//     const mwav::aprs_packet::Telemetry &telemetry_data);

// bool AprsEncodeTelemetryParamUnits(
//     WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
//     const mwav::aprs_packet::Telemetry &telemetry_data);

// bool AprsEncodeTelemetryCoefs(
//     WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
//     const mwav::aprs_packet::Telemetry &telemetry_data);

// bool AprsEncodeTelemetryBitSenseProjName(
//     WavGen &wavgen, const mwav::AprsRequiredFields &required_fields,
//     const mwav::aprs_packet::Telemetry &telemetry_data);

// bool AprsEncodeMessage(WavGen &wavgen,
//                        const mwav::AprsRequiredFields &required_fields,
//                        const mwav::aprs_packet::Message &message_data);

// bool AprsUserDefined(WavGen &wavgen,
//                      const mwav::AprsRequiredFields &required_fields,
//                      const mwav::aprs_packet::UserDefined
//                      &user_defined_data);

// bool AprsExperimental(WavGen &wavgen,
//                       const mwav::AprsRequiredFields &required_fields,
//                       const mwav::aprs_packet::Experimental
//                       &experimental_data);

// bool AprsInvalidPacket(WavGen &wavgen,
//                        const mwav::AprsRequiredFields &required_fields,
//                        const mwav::aprs_packet::Invalid
//                        &invalid_packet_data);

#if SSTV_ENABLED

bool SstvEncode(WavGen &wavgen, const std::string &input_image_path,
                const std::string &callsign, const mwav::Sstv_Mode mode,
                const std::vector<std::string> &comments,
                std::string out_image_path, const bool save_out_image);

#endif // SSTV_ENABLED

} // namespace modulators
#endif // MODULATORS_H_