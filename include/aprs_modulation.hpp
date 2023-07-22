/**
 * @file aprs_modulation.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Public interface for aprs modulation.
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_APRS_MODULATION_HPP_
#define MWAV_APRS_MODULATION_HPP_

namespace mwav::aprs {

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
      0;                           // One character User Defined Packet Type
  std::vector<uint8_t> data = {};  // 1-252
};

struct Experimental {
  std::vector<uint8_t> data = {};  // 1-253 bytes
};

struct Invalid {
  std::vector<uint8_t> data = {};  // 1-254 bytes
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

  std::string telem_destination_address = "   ";  // between 3 and 9 chars

  struct Analog {
    std::string value = "";

    std::string name = "";  // 1 - max_name_length characters
    std::string unit = "";  // 1 - max_name_length characters
    // a*x^2 + b*x + c
    std::string coef_a = "0";  // 1-9 characters, -, ., 0-9
    std::string coef_b = "1";  // 1-9 characters, -, ., 0-9
    std::string coef_c = "0";  // 1-9 characters, -, ., 0-9
    const unsigned int upper_limit = 0;
    Analog(int upper_limit) : upper_limit(upper_limit) {
    }
  };

  struct Digital {
    bool value = false;

    std::string name = "";  // 1 - max_name_length characters
    std::string unit = "";  // 1 - max_name_length characters
    const unsigned int upper_limit = 0;
    Digital(int upper_limit) : upper_limit(upper_limit) {
    }
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

}  // namespace mwav::aprs

#endif /* MWAV_APRS_MODULATION_HPP_ */