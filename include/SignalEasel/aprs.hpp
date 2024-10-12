/// =*=======================SignalEasel=======================*=
/// A friendly library for signal modulation and demodulation.
/// Learn more at https://signaleasel.joshuajer.red
///
/// @copyright Copyright 2024 Joshua Jerred. All rights reserved.
/// This project is licensed under the GNU GPL v3.0 license.
/// =*=========================================================*=

#ifndef SIGNAL_EASEL_APRS_HPP_
#define SIGNAL_EASEL_APRS_HPP_

#include <cstdint>
#include <string>

#include <BoosterSeat/time.hpp>

#include <SignalEasel/afsk.hpp>
#include <SignalEasel/aprs/packets.hpp>
#include <SignalEasel/aprs/telemetry_transcoder.hpp>
#include <SignalEasel/ax25.hpp>

namespace signal_easel::aprs {

struct Settings : public afsk::Settings {
  Settings() {
    /// @brief APRS uses NRZI encoding.
    bit_encoding = BitEncoding::NRZI;
    /// @todo Disable SYN/EOT wrapping for APRS.
    include_ascii_padding = false;
  }
};

class Modulator : public afsk::Modulator {
public:
  Modulator(aprs::Settings settings = aprs::Settings())
      : afsk::Modulator(settings), settings_(settings) {}

  void encode(const aprs::PositionPacket &packet);
  void encode(const aprs::MessagePacket &packet);
  void encode(const aprs::MessageAckPacket &packet);
  void encode(const aprs::ExperimentalPacket &packet);
  void encode(const aprs::TelemetryPacket &telemetryData);

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

  bool parseExperimentalPacket(aprs::ExperimentalPacket &experimental);

  /// @brief If the packet was a telemetry packet, this function will try to
  /// parse the packet and populate the telemetry data object.
  /// @param packet - The telemetry packet to populate
  /// @return \c true if the packet was successfully parsed, \c false otherwise.
  bool parseTelemetryPacket(aprs::TelemetryPacket &packet);

  void printFrame();

private:
  void populateGenericFields(aprs::Packet &packet) const;

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
    uint32_t total_telemetry_packets = 0;
    uint32_t num_telemetry_packets_failed = 0;
    uint32_t total_other_packets = 0;
    uint32_t current_message_packets_in_queue = 0;
    uint32_t current_position_packets_in_queue = 0;
    uint32_t current_experimental_packets_in_queue = 0;
    uint32_t current_telemetry_packets_in_queue = 0;
    uint32_t current_other_packets_in_queue = 0;
  };

  Receiver(aprs::Settings settings = aprs::Settings())
      : afsk::Receiver(settings) {}

  bool getAprsMessage(aprs::MessagePacket &message_packet, ax25::Frame &frame);

  bool getAprsPosition(aprs::PositionPacket &position_packet,
                       ax25::Frame &frame);

  bool getAprsExperimental(aprs::ExperimentalPacket &experimental_packet,
                           ax25::Frame &frame);

  bool getAprsTelemetry(aprs::TelemetryPacket &telemetry_packet,
                        ax25::Frame &frame);

  bool getOtherAprsPacket(ax25::Frame &frame);

  double getSNR() { return demodulation_res_.snr; }

  Stats getStats() { return stats_; }

private:
  void decode() override;
  std::vector<std::pair<ax25::Frame, aprs::MessagePacket>> aprs_messages_{};
  std::vector<std::pair<ax25::Frame, aprs::PositionPacket>> aprs_positions_{};
  std::vector<std::pair<ax25::Frame, aprs::ExperimentalPacket>>
      aprs_experimental_{};
  std::vector<std::pair<ax25::Frame, aprs::TelemetryPacket>> aprs_telemetry_{};
  std::vector<ax25::Frame> other_aprs_packets_{};

  Stats stats_{};

  afsk::Demodulator::ProcessResults demodulation_res_{};

  Demodulator aprs_demodulator_{};
};

/// @brief Encodes a string into a base91 encoded string.
/// @param value - The value to encode.
/// @param num_bytes - The number of bytes to encode the value into.
/// @return The base91 encoded value, of size num_bytes.
std::vector<uint8_t> base91Encode(int value, unsigned int num_bytes);

/// @brief Decodes a base91 encoded value to an integer.
/// @param encoded - A vector of bytes that represent the base91 encoded value.
/// @return The decoded integer.
int base91Decode(std::vector<uint8_t> encoded);

/// @brief The base function to encode an APRS packet with the provided base
/// data and info field.
/// @param required_fields - The required fields for the packet, a base APRS
/// packet.
/// @param info - The information field of the APRS packet.
/// @return The encoded APRS packet.
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