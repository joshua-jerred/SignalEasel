#ifndef MWAV_H_
#define MWAV_H_

#include <exception>

#include "wavgen.h"

namespace mwav {
// -----------------------------------------------------------------------------
// Structs
// -----------------------------------------------------------------------------
/**
 * @brief The modulation modes for data encoding.
 */
enum class DataModulation {
  BPSK_125,  /**< BPSK 125 baud */
  BPSK_250,  /**< BPSK 250 baud */
  BPSK_500,  /**< BPSK 500 baud */
  BPSK_1000, /**< BPSK 1000 baud */
  QPSK_125,  /**< QPSK 125 baud */
  QPSK_250,  /**< QPSK 250 baud */
  QPSK_500,  /**< QPSK 500 baud */
  AFSK1200   /**< AFSK 1200 baud */
};

enum class AprsSymbolTable { PRIMARY, SECONDARY };
struct AprsRequiredFields {
  std::string source_address = "";  // 3 - 6 characters, your callsign
  uint8_t source_ssid = 0;          // 0 - 15

  std::string destination_address = "APZMWV";  // Default is APRS
  uint8_t destination_ssid = 0;              // 0 - 15

  AprsSymbolTable symbol_table = AprsSymbolTable::PRIMARY;
  char symbol = '/';  // Symbol character default is dot (//)

  bool location_data = false;
  bool telemetry_data = false;
};

namespace aprs_packet {
struct AprsLocationData {
  std::string time_code = "";  // hhmmss
  float latitude = 0.0;        // Decimal degrees
  float longitude = 0.0;       // Decimal degrees
  int altitude = 0;            // Feet 0 - 99999
  float speed = 0.0;           // Knots 0 - 400
  int course = 0;              // Degrees 0 - 359

  // A comment to add to the end of the packet 
  std::string comment = ""; /** @todo max length of comment */
};
} // namespace mwav::aprs_packet

struct AprsTelemetryData {};

// -----------------------------------------------------------------------------
// Encoding
// -----------------------------------------------------------------------------
/**
 * @brief Encode a string of data into BPSK/QPSK/AFSK1200 audio, with or without
 * a morse code callsign.
 *
 * @details This function will encode a string of data into a wav file depending
 * on the modulation type. The input string must be ASCII characters only. This
 * will generate a wav file at the specified path.
 *
 * If an optional callsign is specified then it will be encoded with morse code
 * at the end of the transmission. This is highly recommended, and possibly
 * legally required, if you are transmitting on ham radio frequencies.
 *
 * If the modulation mode is BPSK/QPSK then it will be encoded with the
 * PSK31 standard (varicode and convolutional coding). This means that it
 * can be decoded with Fldigi or other software that supports PSK31.
 *
 * With AFSK1200 it will *not* be encoded with AX.25 or NRZI, simply just
 * raw ASCII with 1200hz tones for 0 and 2200hz tones for 1. This will
 * support minimodem and other software that supports Bell 202 style AFSK.
 *
 * @param modulation The modulation mode to use.
 * @param input A string of data to encode, ASCII only.
 * @param out_file_path The path to the output wav file.
 * @param callsign *Optional* The callsign to use, must be 3-6 characters or
 * left blank.
 * @return true - Success
 * @return false - Failure
 * @exception mwav::Exception
 */
bool EncodeString(const DataModulation modulation, const std::string input,
            const std::string out_file_path,
            const std::string callsign = "NOCALLSIGN");

/**
 * @brief Encode a raw binary array of data into BPSK/QPSK/AFSK1200 audio with
 * a morse code callsign at the start.
 *
 * @details This function will take a raw binary array of data and encode it.
 * It will not be converted to ASCII. With BPSK/QPSK it will not be to the PSK31
 * spec, it will be raw binary. The same goes for AFSK1200.
 *
 * If an optional callsign is specified then it will be encoded with morse code
 * at the end of the transmission. This is highly recommended, and probably
 * legally required, if you are transmitting on ham radio frequencies.
 *
 * A callsign is *highly* recommended with these modes for legal identification
 * if transmitting due to this being off spec.
 *
 * @param modulation The modulation mode to use.
 * @param input Raw binary data to encode in the form of unsigned bytes.
 * @param input_length The number of bytes in the input array.
 * @param out_file_path The path to the output wav file.
 * @param callsign The callsign to use, must be 3-6 characters.
 * @return true - Success
 * @return false - Failure
 * @exception mwav::Exception
 */
bool EncodeRawData(const DataModulation modulation, const unsigned char* input,
            const int input_length, const std::string out_file_path,
            const std::string callsign = "NOCALLSIGN");


// APRS ---------------------------------------------------------------
bool EncodeAprs(
            const std::string out_file_path,
            const AprsRequiredFields required_fields,
            const aprs_packet::AprsLocationData location = {},
            const AprsTelemetryData telemetry = {},
            const std::string morse_callsign = "NOCALLSIGN");
}  // namespace mwav

#endif  // MWAV_H_