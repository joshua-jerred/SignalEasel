/**
 * @file mwav.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief The main file for the MWAV library.
 * @see mwav.h for detailed documentation.
 * @date 2022-12-22
 * @copyright Copyright (c) 2022
 * @version 0.1
 */

#include "mwav.h"

#include "modulators.h"
#include "mwav-exception.h"

void addCall(WavGen &wavgen, const std::string morse_callsign) {
  if (morse_callsign != "NOCALLSIGN" &&
      morse_callsign != "") {       // Callsign specified
    wavgen.addSineWave(0, 0, 0.5);  // Add a 0.5 second pause
    if (!modulators::EncodeMorse(wavgen, morse_callsign)) {
      throw mwav::Exception("Failed to encode morse callsign.");
    };
  }
}

/** @todo need to check return value of the modulators functions */

// -------------------------------- PSK/AFSK ---------------------------------

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

// -------------------------------- APRS ---------------------------------
// Location
bool mwav::EncodeAprs(
    const std::string out_file_path,
    const mwav::AprsRequiredFields required_fields,
    const mwav::aprs_packet::CompressedPositionReport location,
    const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  modulators::AprsEncodePositionPacket(wavgen, required_fields, location);
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// Telemetry
bool mwav::EncodeAprs(const std::string out_file_path,
                      const AprsRequiredFields required_fields,
                      const aprs_packet::Telemetry telem,
                      const aprs_packet::TelemetryPacketType packet_type,
                      const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  if (packet_type == aprs_packet::TelemetryPacketType::DATA_REPORT) {
    modulators::AprsEncodeTelemetryData(wavgen, required_fields, telem);
  } else if (packet_type == aprs_packet::TelemetryPacketType::PARAM_NAME) {
    modulators::AprsEncodeTelemetryParamName(wavgen, required_fields, telem);
  } else if (packet_type == aprs_packet::TelemetryPacketType::PARAM_UNIT) {
    modulators::AprsEncodeTelemetryParamUnits(wavgen, required_fields, telem);
  } else if (packet_type == aprs_packet::TelemetryPacketType::PARAM_COEF) {
    modulators::AprsEncodeTelemetryCoefs(wavgen, required_fields, telem);
  } else if (packet_type ==
             aprs_packet::TelemetryPacketType::BIT_SENSE_PROJ_NAME) {
    modulators::AprsEncodeTelemetryBitSenseProjName(wavgen, required_fields,
                                                    telem);
  } else {
    throw mwav::Exception("Invalid telemetry packet type.");
  }
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// Message
bool mwav::EncodeAprs(const std::string out_file_path,
                      const AprsRequiredFields required_fields,
                      const aprs_packet::Message message,
                      const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  modulators::AprsEncodeMessage(wavgen, required_fields, message);
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// Message Acknowledgement
bool mwav::EncodeAprs(const std::string out_file_path,
                      const AprsRequiredFields required_fields,
                      const aprs_packet::MessageAck message_ack,
                      const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  mwav::aprs_packet::Message message = mwav::aprs_packet::Message();
  message.addressee = message_ack.addressee;
  int message_id_size = message_ack.message_id.size();
  if (message_id_size < 1 || message_id_size < 5) {
    mwav::Exception("Invalid message ID size.");
  }
  message.message = "ack" + message_ack.message_id;
  modulators::AprsEncodeMessage(wavgen, required_fields, message);
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// Message No Acknowledgement
bool mwav::EncodeAprs(const std::string out_file_path,
                      const AprsRequiredFields required_fields,
                      const aprs_packet::MessageNack message_no_ack,
                      const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  mwav::aprs_packet::Message message = mwav::aprs_packet::Message();
  message.addressee = message_no_ack.addressee;
  int message_id_size = message_no_ack.message_id.size();
  if (message_id_size < 1 || message_id_size < 5) {
    mwav::Exception("Invalid message ID size.");
  }
  message.message = "rej" + message_no_ack.message_id;
  modulators::AprsEncodeMessage(wavgen, required_fields, message);
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// User Defined
bool mwav::EncodeAprs(const std::string out_file_path,
                      const AprsRequiredFields required_fields,
                      const aprs_packet::UserDefined user_defined,
                      const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  modulators::AprsUserDefined(wavgen, required_fields, user_defined);
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// Experimental
bool mwav::EncodeAprs(const std::string out_file_path,
                      const AprsRequiredFields required_fields,
                      const aprs_packet::Experimental experimental,
                      const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  modulators::AprsExperimental(wavgen, required_fields, experimental);
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// Invalid
bool mwav::EncodeAprs(const std::string out_file_path,
                      const AprsRequiredFields required_fields,
                      const aprs_packet::Invalid invalid,
                      const std::string morse_callsign) {
  WavGen wavgen = WavGen(out_file_path);
  modulators::AprsInvalidPacket(wavgen, required_fields, invalid);
  addCall(wavgen, morse_callsign);
  wavgen.done();
  return true;
}

// ------------------------------- SSTV ---------------------------------
#if SSTV_ENABLED

bool mwav::EncodeSSTV(const std::string &out_file_path,
                      const std::string &input_image_path,
                      const bool save_out_image,
                      const std::string &callsign, 
                      const mwav::Sstv_Mode mode,
                      const std::vector<std::string> &comments,
                      std::string out_image_path,
                      const bool morse_callsign) {
  if (callsign.size() > 10) { /** @todo Arbitrary, needs to be changed later */
    throw mwav::Exception("Callsign too long.");
  }
  if (comments.size() > 40) { /** @todo Arbitrary, needs to be changed later */
    throw mwav::Exception("Comments vector too long.");
  }

  for (std::string comment : comments) {
    if (comment.size() > 15) { /** @todo Arbitrary, needs to be changed later */
      throw mwav::Exception("Comment too long." + comment);
    }
  }

  WavGen wavgen = WavGen(out_file_path, 48000); // 48 kHz, divisible by robot sync times
  modulators::SstvEncode(wavgen, input_image_path, callsign, mode, comments,
                         out_image_path, save_out_image);
  if (morse_callsign) {
    addCall(wavgen, callsign);
  }
  wavgen.done();
  return true;
}

#endif