/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file       packets.hpp
 * @date       2024-08-17
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <BoosterSeat/time.hpp>

#include <SignalEasel/aprs/telemetry_data.hpp>

namespace signal_easel::aprs {

/// @brief The base APRS packet structure.
struct Packet {
  enum class SymbolTable { PRIMARY, SECONDARY };
  enum class Type {
    UNKNOWN,
    POSITION,
    MESSAGE,
    EXPERIMENTAL,
    TELEMETRY_DATA_REPORT,
    TELEMETRY_COEFFICIENT,
    TELEMETRY_PARAMETER_NAME,
    TELEMETRY_PARAMETER_UNIT,
    TELEMETRY_BIT_SENSE_PROJ_NAME,
  };

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

struct MessageAckPacket : public Packet {
  std::string addressee = "";  // 3-9 characters
  std::string message_id = ""; // 1-5 characters
  std::vector<uint8_t> encode() const;
};

/// @brief Raw packet with two '{' characters at the start.
struct ExperimentalPacket : Packet {
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

struct TelemetryPacket : Packet {
  TelemetryPacket() = default;
  TelemetryPacket(Packet packet, telemetry::TelemetryData data = {})
      : Packet(packet), telemetry_data(data) {}
  TelemetryPacket(telemetry::TelemetryData data) : telemetry_data(data) {}

  telemetry::TelemetryData telemetry_data{};

  Packet::Type telemetry_type = Packet::Type::UNKNOWN;

  std::vector<uint8_t> encode() const;
};

}; // namespace signal_easel::aprs