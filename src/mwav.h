#ifndef MWAV_H_
#define MWAV_H_

#include <exception>
#include <vector>

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
  uint8_t destination_ssid = 0;                // 0 - 15

  AprsSymbolTable symbol_table = AprsSymbolTable::PRIMARY;
  char symbol = '/';  // Symbol character default is dot (//)
};

namespace aprs_packet {
struct CompressedPositionReport {
  std::string time_code = "";  // hhmmss
  float latitude = 0.0;        // Decimal degrees
  float longitude = 0.0;       // Decimal degrees
  int altitude = 0;            // Feet 0 - 99999
  float speed = 0.0;           // Knots 0 - 400
  int course = 0;              // Degrees 0 - 359

  // A comment to add to the end of the packet
  std::string comment = ""; /** @todo max length of comment */
};

struct Message {
  std::string addressee = "";   // 3-9 characters
  std::string message = "";     // Max length of 67 characters
  std::string message_id = "";  // Optional, 1-5 characters
};

struct MessageAck {
  std::string addressee = "";   // 3-9 characters
  std::string message_id = "";  // 1-5 characters
};

struct MessageNack {
  std::string addressee = "";   // 3-9 characters
  std::string message_id = "";  // 1-5 characters
};

struct UserDefined {
  unsigned char UserId = 0;  // One character User ID
  unsigned char UserDefPacketType =
      0;                      // One character User Defined Packet Type
  std::vector<uint8_t> data;  // 1-252
};

struct Experimental {
  std::vector<uint8_t> data;  // 1-253 bytes
};

struct Invalid {
  std::vector<uint8_t> data;  // 1-254 bytes
};

enum class TelemetryPacketType {
  DATA_REPORT,
  PARAM_NAME,
  PARAM_UNIT,
  PARAM_COEF,
  BIT_SENSE_PROJ_NAME
};

struct Telemetry {
  // The sequence number and comment are use in the DATA_REPORT packet
  std::string sequence_number = "001";  // 3 digits
  std::string comment = "";             // Max length of 220 characters

  // The Project Title is used in the BIT_SENSE_PROJ_NAME packet
  std::string project_title = "";  // 0-23 characters

  std::string telem_destination_address = "   "; // between 3 and 9 chars

  struct Analog {
    std::string value = "";

    std::string name = "";  // 1 - max_name_length characters
    std::string unit = "";  // 1 - max_name_length characters
    // a*x^2 + b*x + c
    std::string coef_a = "0";  // 1-9 characters, -, ., 0-9
    std::string coef_b = "1";  // 1-9 characters, -, ., 0-9
    std::string coef_c = "0";  // 1-9 characters, -, ., 0-9
    const unsigned int upper_limit = 0;
    Analog(int upper_limit) : upper_limit(upper_limit) {}
  };

  struct Digital {
    bool value = false;

    std::string name = "";  // 1 - max_name_length characters
    std::string unit = "";  // 1 - max_name_length characters
    const unsigned int upper_limit = 0;
    Digital(int upper_limit) : upper_limit(upper_limit) {}
  };

  Analog A1 = Analog(7);
  Analog A2 = Analog(7);
  Analog A3 = Analog(6);
  Analog A4 = Analog(6);
  Analog A5 = Analog(5);

  Digital D1 = Digital(7);
  Digital D2 = Digital(7);
  Digital D3 = Digital(6);
  Digital D4 = Digital(6);
  Digital D5 = Digital(5);
  Digital D6 = Digital(5);
  Digital D7 = Digital(5);
  Digital D8 = Digital(5);
};
}  // namespace aprs_packet

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
// Compressed Position Report (GPS)
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::CompressedPositionReport location,
                const std::string morse_callsign = "NOCALLSIGN");

// Telemetry (Data, Parameter names, Units, Coefficients, or Bitfields/Proj
// Name)
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Telemetry telemetry,
                const aprs_packet::TelemetryPacketType packet_type,
                const std::string morse_callsign = "NOCALLSIGN");

// Message
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Message message,
                const std::string morse_callsign = "NOCALLSIGN");

// Message ACK
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::MessageAck message_ack,
                const std::string morse_callsign = "NOCALLSIGN");

// Message NACK
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::MessageNack message_nack,
                const std::string morse_callsign = "NOCALLSIGN");

// User Defined APRS Packet
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::UserDefined user_defined_packet,
                const std::string morse_callsign = "NOCALLSIGN");

// Experimental
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Experimental experimental_packet,
                const std::string morse_callsign = "NOCALLSIGN");

// Invalid
bool EncodeAprs(const std::string out_file_path,
                const AprsRequiredFields required_fields,
                const aprs_packet::Invalid invalid_packet,
                const std::string morse_callsign = "NOCALLSIGN");

}  // namespace mwav

#endif  // MWAV_H_