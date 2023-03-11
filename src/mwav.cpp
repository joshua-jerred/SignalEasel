/**
 * @file mwav.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief The main file for the MWAV library.
 * @see mwav.h for detailed documentation.
 * @date 2022-12-22
 * @copyright Copyright (c) 2022
 * @version 0.1
 */

#include "mwav-exception.h"
#include "mwav.h"
#include "modulators.h"

bool mwav::EncodeString(const mwav::DataModulation modulation,
                        const std::string input,
                        const std::string out_file_path,
                        const std::string callsign) {
  WavGen wavgen = WavGen(out_file_path);

  try {
    if (modulation == mwav::DataModulation::AFSK1200) {
      return modulators::AfskEncodeAscii(wavgen, input);
    } else {
      return modulators::PskEncodeAscii(wavgen, input, modulation);
    }

    if (callsign != "NOCALLSIGN" && callsign != "") {  // Callsign specified
      wavgen.addSineWave(0, 0, 0.5);                   // Add a 0.5 second pause
      if (!modulators::EncodeMorse(wavgen, callsign)) {
        return false;
      };
    }
  } catch (const mwav::Exception &e) {
    throw mwav::Exception("Error encoding data: " + std::string(e.what()));
  }
  return true;
}

// -------------------------------- APRS INPUT ---------------------------------
bool mwav::EncodeAprs(
    const std::string out_file_path,
    const mwav::AprsRequiredFields required_fields,
    const mwav::aprs_packet::AprsLocationData location,
    const mwav::AprsTelemetryData telemetry,
    const std::string morse_callsign) {

  (void) morse_callsign;

  bool location_data = required_fields.location_data;
  bool telemetry_data = required_fields.telemetry_data;

  if (!location_data && !telemetry_data) {
    throw mwav::Exception("No location or telemetry data specified.");
  }

  WavGen wavgen = WavGen(out_file_path);
  try {
    modulators::AprsEncodePacket(wavgen, required_fields, location, telemetry);
  } catch (mwav::Exception &e) {
    throw mwav::Exception("Error encoding APRS packet: " + std::string(e.what()));
  }

  if (morse_callsign != "NOCALLSIGN" && morse_callsign != "") {  // Callsign specified
    wavgen.addSineWave(0, 0, 0.5);                   // Add a 0.5 second pause
    if (!modulators::EncodeMorse(wavgen, morse_callsign)) {
      return false;
    };
  }
  
  wavgen.done();
  return true;
}