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
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  encodeBytes(output_bytes);
}

void Modulator::encode(const aprs::MessagePacket &packet) {
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  encodeBytes(output_bytes);
}

void Modulator::encode(const aprs::MessageAck &packet) {
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  encodeBytes(output_bytes);
}

void Modulator::encode(const aprs::Experimental &packet) {
  std::vector<uint8_t> info = packet.encode();
  std::vector<uint8_t> output_bytes = encodePacket(packet, info);
  encodeBytes(output_bytes);
}

} // namespace signal_easel::aprs