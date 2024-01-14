/**
 * @file aprs_modulator.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of the APRS modulator class.
 * @date 2024-01-14
 * @copyright Copyright (c) 2024
 */

#include <SignalEasel/aprs.hpp>

namespace signal_easel::aprs {

void Modulator::encodePositionPacket(aprs::PositionPacket packet) {
  std::vector<uint8_t> info;
  addLocationData(packet, settings_.base_packet, info);
  std::vector<uint8_t> output_bytes = encodePacket(settings_.base_packet, info);

  encodeBytes(output_bytes);
}

void Modulator::encodeMessagePacket(aprs::MessagePacket message) {
  std::vector<uint8_t> info = message.encode();
  std::vector<uint8_t> output_bytes = encodePacket(settings_.base_packet, info);
  encodeBytes(output_bytes);
}

void Modulator::encodeMessageAck(aprs::MessageAck ack) {
  std::vector<uint8_t> info = ack.encode();
  std::vector<uint8_t> output_bytes = encodePacket(settings_.base_packet, info);
  encodeBytes(output_bytes);
}

} // namespace signal_easel::aprs