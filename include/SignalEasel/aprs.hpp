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

#include <BoosterSeat/time.hpp>

#include <SignalEasel/afsk.hpp>
#include <SignalEasel/ax25.hpp>

namespace signal_easel::aprs {

struct Packet {
  enum class SymbolTable { PRIMARY, SECONDARY };
  enum class Type { UNKNOWN, POSITION, MESSAGE, EXPERIMENTAL };

  std::string source_address = ""; // 3 - 6 characters, your callsign
  uint8_t source_ssid = 0;         // 0 - 15

  std::string destination_address = "APZMWV"; // Default is APRS
  uint8_t destination_ssid = 0;               // 0 - 15

  SymbolTable symbol_table = SymbolTable::PRIMARY;
  char symbol = '/'; // Symbol character default is dot (//)
};

struct PositionPacket : public Packet {
  std::string time_code = ""; // ddhhmm in *UTC* specifically.
  float latitude = 0.0;       // Decimal degrees
  float longitude = 0.0;      // Decimal degrees
  int altitude = 0;           // Feet 0 - 99999
  float speed = 0.0;          // Knots 0 - 400
  int course = 0;             // Degrees 0 - 359

  // A comment to add to the end of the packet
  std::string comment = ""; // max length of 40 characters

  ax25::Frame frame{};

  /**
   * @brief This timestamp is set after the packet is decoded. It's used to
   * keep track of the age of the packet.
   */
  bst::Time decoded_timestamp{};

  std::vector<uint8_t> encode() const;
};

struct MessagePacket : public Packet {
  std::string addressee = "";  // 3-9 characters
  std::string message = "";    // Max length of 67 characters
  std::string message_id = ""; // Optional, 1-5 characters

  std::vector<uint8_t> encode() const;
};

struct MessageAck : public Packet {
  std::string addressee = "";  // 3-9 characters
  std::string message_id = ""; // 1-5 characters
  std::vector<uint8_t> encode() const;
};

struct Settings : public afsk::Settings {
  Settings() : base_packet{} {
    bit_encoding = BitEncoding::NRZI;
    include_ascii_padding = false;
  }
  Packet base_packet;
};

/**
 * @brief This packet type will have two `{` characters at the start.
 */
struct Experimental : Packet {
  unsigned char packet_type_char = 'a'; // One character packet type
  std::vector<uint8_t> data = {};       // 1-252 bytes

  std::string getStringData() const {
    return std::string(data.begin(), data.end());
  }

  void setStringData(std::string data_str) {
    data = std::vector<uint8_t>(data_str.begin(), data_str.end());
  }

  std::vector<uint8_t> encode() const;
};

class Modulator : public afsk::Modulator {
public:
  Modulator(aprs::Settings settings = aprs::Settings())
      : afsk::Modulator(settings), settings_(settings) {}

  void setCallSign(std::string call_sign) {
    settings_.base_packet.source_address = call_sign;
  }

  void encode(const aprs::PositionPacket &packet);
  void encode(const aprs::MessagePacket &packet);
  void encode(const aprs::MessageAck &packet);
  void encode(const aprs::Experimental &packet);

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
   * @brief If the packet was a message packet, this function will try to
   * parse the packet into a message packet.
   * @param message (out) The message packet
   * @return true if the packet was a valid message packet
   */
  bool parseMessagePacket(aprs::MessagePacket &message);

  /**
   * @brief If the packet was a position packet, this function will try to
   * parse the packet into it's positional data.
   * @todo This function will not be friendly to many types of aprs position
   * packets.
   * @param position (out) The position packet
   * @return true if the packet was fully parsed
   * @return false if the packet was not a valid position packet
   */
  bool parsePositionPacket(aprs::PositionPacket &position);

  bool parseExperimentalPacket(aprs::Experimental &experimental);

  void printFrame();

private:
  ax25::Frame frame_{};
  aprs::Packet::Type type_ = aprs::Packet::Type::UNKNOWN;
};

class Receiver : public signal_easel::afsk::Receiver {
public:
  struct Stats {
    uint32_t total_message_packets = 0;
    uint32_t num_message_packets_failed = 0;
    uint32_t total_position_packets = 0;
    uint32_t num_position_packets_failed = 0;
    uint32_t total_experimental_packets = 0;
    uint32_t num_experimental_packets_failed = 0;
    uint32_t total_other_packets = 0;
    uint32_t current_message_packets_in_queue = 0;
    uint32_t current_position_packets_in_queue = 0;
    uint32_t current_other_packets_in_queue = 0;
  };

  Receiver(aprs::Settings settings = aprs::Settings())
      : afsk::Receiver(settings) {}

  bool getAprsMessage(aprs::MessagePacket &message_packet, ax25::Frame &frame);

  bool getAprsPosition(aprs::PositionPacket &position_packet,
                       ax25::Frame &frame);

  bool getAprsExperimental(aprs::Experimental &experimental_packet,
                           ax25::Frame &frame);

  bool getOtherAprsPacket(ax25::Frame &frame);

  double getSNR() { return demodulation_res_.snr; }

  Stats getStats() { return stats_; }

private:
  void decode() override;
  std::vector<std::pair<ax25::Frame, aprs::MessagePacket>> aprs_messages_{};
  std::vector<std::pair<ax25::Frame, aprs::PositionPacket>> aprs_positions_{};
  std::vector<std::pair<ax25::Frame, aprs::Experimental>> aprs_experimental_{};
  std::vector<ax25::Frame> other_aprs_packets_{};

  Stats stats_{};

  afsk::Demodulator::ProcessResults demodulation_res_{};

  Demodulator aprs_demodulator_{};
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
  std::string sequence_number = "001"; // 3 digits
  std::string comment = "";            // Max length of 220 characters

  // The Project Title is used in the BIT_SENSE_PROJ_NAME packet
  std::string project_title = ""; // 0-23 characters

  std::string destination_address = "   "; // between 3 and 9 chars

  struct Analog {
    uint8_t value = 0;

    // the length differs for each value.
    std::string name = ""; // 1 - max_name_length characters
    std::string unit = ""; // 1 - max_name_length characters
    // a*x^2 + b*x + c
    std::string coef_a = "0"; // 1-9 characters, -, ., 0-9
    std::string coef_b = "1"; // 1-9 characters, -, ., 0-9
    std::string coef_c = "0"; // 1-9 characters, -, ., 0-9
  };

  struct Digital {
    bool value = false;
    bool sense = false; // for BIT_SENSE_PROJ_NAME packet

    std::string name = ""; // 1 - max_name_length characters
    std::string unit = ""; // 1 - max_name_length characters
  };

  Analog a1{};
  Analog a2{};
  Analog a3{};
  Analog a4{};
  Analog a5{};

  Digital d1{};
  Digital d2{};
  Digital d3{};
  Digital d4{};
  Digital d5{};
  Digital d6{};
  Digital d7{};
  Digital d8{};

  std::vector<uint8_t>
  encode(TelemetryPacketType type = TelemetryPacketType::DATA_REPORT) const;
};

std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes);
int base91Decode(std::vector<uint8_t> encoded);
std::vector<uint8_t> encodePacket(const aprs::Packet &required_fields,
                                  std::vector<uint8_t> &info);

} // namespace signal_easel::aprs

// struct MessageNack {
//   std::string addressee = "";  // 3-9 characters
//   std::string message_id = ""; // 1-5 characters
// };

// struct UserDefined {
//   unsigned char UserId = 0;            // One character User ID
//   unsigned char UserDefPacketType = 0; // One character User Defined Packet
//   Type std::vector<uint8_t> data = {};      // 1-252
// };

// struct Invalid {
//   std::vector<uint8_t> data = {}; // 1-254 bytes
// };

#endif /* APRS_HPP_ */