/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   aprs.hpp
 * @date   2023-12-05
 * @brief  APRS implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_APRS_HPP_
#define SIGNAL_EASEL_APRS_HPP_

#include <cstdint>
#include <string>

#include <SignalEasel/afsk.hpp>
#include <SignalEasel/ax25.hpp>

namespace signal_easel::aprs {

struct Packet {
  enum class SymbolTable { PRIMARY, SECONDARY };
  enum class Type { UNKNOWN, POSITION, MESSAGE };

  std::string source_address = ""; // 3 - 6 characters, your callsign
  uint8_t source_ssid = 0;         // 0 - 15

  std::string destination_address = "APZMWV"; // Default is APRS
  uint8_t destination_ssid = 0;               // 0 - 15

  SymbolTable symbol_table = SymbolTable::PRIMARY;
  char symbol = '/'; // Symbol character default is dot (//)
};

struct PositionPacket {
  std::string time_code = ""; // hhmmss
  float latitude = 0.0;       // Decimal degrees
  float longitude = 0.0;      // Decimal degrees
  int altitude = 0;           // Feet 0 - 99999
  float speed = 0.0;          // Knots 0 - 400
  int course = 0;             // Degrees 0 - 359

  // A comment to add to the end of the packet
  std::string comment = ""; /** @todo max length of comment */
};

struct MessagePacket {
  std::string addressee = "";  // 3-9 characters
  std::string message = "";    // Max length of 67 characters
  std::string message_id = ""; // Optional, 1-5 characters

  std::vector<uint8_t> encode() const;
};

struct Settings : public afsk::Settings {
  Settings() : base_packet{} {
    bit_encoding = BitEncoding::NRZI;
    include_ascii_padding = false;
  }
  Packet base_packet;
};

class Modulator : public afsk::Modulator {
public:
  Modulator(aprs::Settings settings = aprs::Settings())
      : afsk::Modulator(settings), settings_(settings) {}

  void setCallSign(std::string call_sign) {
    settings_.base_packet.source_address = call_sign;
  }

  void setBasePacket(aprs::Packet packet) { settings_.base_packet = packet; }

  void encodePositionPacket(aprs::PositionPacket packet);
  void encodeMessagePacket(aprs::MessagePacket message);

private:
  aprs::Settings settings_;
};

class Demodulator : public signal_easel::afsk::Demodulator {
public:
  friend class Receiver;

  Demodulator(afsk::Settings settings = afsk::Settings())
      : afsk::Demodulator(settings) {}

  bool lookForAx25Packet();
  aprs::Packet::Type getType() { return type_; }

  /**
   * @brief If the packet was a message packet, this function will try to parse
   * the packet into a message packet.
   * @param message (out) The message packet
   * @return true if the packet was a valid message packet
   */
  bool parseMessagePacket(aprs::MessagePacket &message);

  void printFrame();

private:
  ax25::Frame frame_{};
  aprs::Packet::Type type_ = aprs::Packet::Type::UNKNOWN;
};

class Receiver : public signal_easel::afsk::Receiver {
public:
  Receiver(aprs::Settings settings = aprs::Settings())
      : afsk::Receiver(settings) {}

  bool getAprsFrame(ax25::Frame &frame) {
    if (aprs_frames_.empty()) {
      return false;
    }
    frame = aprs_frames_.back();
    aprs_frames_.pop_back();
    return true;
  }

private:
  void decode() override;
  std::vector<ax25::Frame> aprs_frames_{};

  Demodulator aprs_demodulator_{};
};

} // namespace signal_easel::aprs

// struct MessageAck {
//   std::string addressee = "";  // 3-9 characters
//   std::string message_id = ""; // 1-5 characters
// };

// struct MessageNack {
//   std::string addressee = "";  // 3-9 characters
//   std::string message_id = ""; // 1-5 characters
// };

// struct UserDefined {
//   unsigned char UserId = 0;            // One character User ID
//   unsigned char UserDefPacketType = 0; // One character User Defined Packet
//   Type std::vector<uint8_t> data = {};      // 1-252
// };

// struct Experimental {
//   std::vector<uint8_t> data = {}; // 1-253 bytes
// };

// struct Invalid {
//   std::vector<uint8_t> data = {}; // 1-254 bytes
// };

// enum class TelemetryPacketType {
//   DATA_REPORT,
//   PARAM_NAME,
//   PARAM_UNIT,
//   PARAM_COEF,
//   BIT_SENSE_PROJ_NAME
// };

// struct Telemetry {
//   // The sequence number and comment are use in the DATA_REPORT packet
//   std::string sequence_number = "001"; // 3 digits
//   std::string comment = "";            // Max length of 220 characters

//   // The Project Title is used in the BIT_SENSE_PROJ_NAME packet
//   std::string project_title = ""; // 0-23 characters

//   std::string telem_destination_address = "   "; // between 3 and 9 chars

//   struct Analog {
//     std::string value = "";

//     std::string name = ""; // 1 - max_name_length characters
//     std::string unit = ""; // 1 - max_name_length characters
//     // a*x^2 + b*x + c
//     std::string coef_a = "0"; // 1-9 characters, -, ., 0-9
//     std::string coef_b = "1"; // 1-9 characters, -, ., 0-9
//     std::string coef_c = "0"; // 1-9 characters, -, ., 0-9
//     const unsigned int upper_limit = 0;
//     Analog(int upper_limit) : upper_limit(upper_limit) {}
//   };

//   struct Digital {
//     bool value = false;

//     std::string name = ""; // 1 - max_name_length characters
//     std::string unit = ""; // 1 - max_name_length characters
//     const unsigned int upper_limit = 0;
//     Digital(int upper_limit) : upper_limit(upper_limit) {}
//   };

//   Analog A1 = Analog(7);
//   Analog A2 = Analog(7);
//   Analog A3 = Analog(6);
//   Analog A4 = Analog(6);
//   Analog A5 = Analog(5);

//   Digital D1 = Digital(7);
//   Digital D2 = Digital(7);
//   Digital D3 = Digital(6);
//   Digital D4 = Digital(6);
//   Digital D5 = Digital(5);
//   Digital D6 = Digital(5);
//   Digital D7 = Digital(5);
//   Digital D8 = Digital(5);
// };

#endif /* APRS_HPP_ */