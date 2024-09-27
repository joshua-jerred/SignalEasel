/**
 * @file aprs_modulator.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the APRS modulator class.
 * @date 2024-01-14
 * @copyright Copyright (c) 2024
 */

#include <SignalEasel/aprs.hpp>

namespace signal_easel::aprs {

void Modulator::encode(const aprs::PositionPacket &packet) {
  // Generate the information bytes
  std::vector<uint8_t> info = packet.encode();
  // Use the information from the packet base class to encode the full packet
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  // Encode with AX.25
  afsk::Modulator::encodeBytes(output_bytes);
}

void Modulator::encode(const aprs::MessagePacket &packet) {
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  afsk::Modulator::encodeBytes(output_bytes);
}

void Modulator::encode(const aprs::MessageAckPacket &packet) {
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  afsk::Modulator::encodeBytes(output_bytes);
}

void Modulator::encode(const aprs::ExperimentalPacket &packet) {
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  afsk::Modulator::encodeBytes(output_bytes);
}

void Modulator::encode(const aprs::TelemetryPacket &packet) {
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  afsk::Modulator::encodeBytes(output_bytes);
}

} // namespace signal_easel::aprs