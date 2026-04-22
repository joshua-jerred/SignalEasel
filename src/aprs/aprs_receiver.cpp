/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   aprs_receiver.cpp
 * @date   2023-12-10
 * @brief  Implementation of the APRS receiver class.
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/aprs.hpp>
#include <SignalEasel/exception.hpp>

#include <iomanip>
#include <iostream>

namespace signal_easel::aprs {

bool Receiver::getAprsMessage(aprs::MessagePacket &message_packet,
                              ax25::Frame &frame) {
  if (aprs_messages_.empty()) {
    return false;
  }
  frame = aprs_messages_.back().first;
  message_packet = aprs_messages_.back().second;
  aprs_messages_.pop_back();
  return true;
}

bool Receiver::getAprsPosition(aprs::PositionPacket &position_packet,
                               ax25::Frame &frame) {
  if (aprs_positions_.empty()) {
    return false;
  }
  frame = aprs_positions_.back().first;
  position_packet = aprs_positions_.back().second;
  aprs_positions_.pop_back();
  return true;
}

bool Receiver::getAprsExperimental(
    aprs::ExperimentalPacket &experimental_packet, ax25::Frame &frame) {
  if (aprs_experimental_.empty()) {
    return false;
  }
  frame = aprs_experimental_.back().first;
  experimental_packet = aprs_experimental_.back().second;
  aprs_experimental_.pop_back();
  return true;
}

bool Receiver::getAprsTelemetry(aprs::TelemetryPacket &telemetry_packet,
                                ax25::Frame &frame) {
  if (aprs_telemetry_.empty()) {
    return false;
  }
  frame = aprs_telemetry_.back().first;
  telemetry_packet = aprs_telemetry_.back().second;
  aprs_telemetry_.pop_back();
  return true;
}

bool Receiver::getOtherAprsPacket(ax25::Frame &frame) {
  if (other_aprs_packets_.empty()) {
    return false;
  }
  frame = other_aprs_packets_.back();
  other_aprs_packets_.pop_back();
  return true;
}

void Receiver::processDecodedFrame() {
  const auto type = aprs_demodulator_.getType();

  switch (type) {
  case aprs::Packet::Type::MESSAGE: {
    aprs::MessagePacket message_packet;
    if (aprs_demodulator_.parseMessagePacket(message_packet)) {
      aprs_messages_.emplace_back(aprs_demodulator_.frame_, message_packet);
      stats_.total_message_packets++;
    } else {
      stats_.num_message_packets_failed++;
      other_aprs_packets_.push_back(aprs_demodulator_.frame_);
      stats_.total_other_packets++;
    }
    break;
  }
  case aprs::Packet::Type::POSITION: {
    aprs::PositionPacket position_packet;
    if (aprs_demodulator_.parsePositionPacket(position_packet)) {
      position_packet.decoded_timestamp.setToNow();
      aprs_positions_.emplace_back(aprs_demodulator_.frame_, position_packet);
      stats_.total_position_packets++;
    } else {
      stats_.num_position_packets_failed++;
      other_aprs_packets_.push_back(aprs_demodulator_.frame_);
      stats_.total_other_packets++;
    }
    break;
  }
  case aprs::Packet::Type::EXPERIMENTAL: {
    aprs::ExperimentalPacket experimental_packet;
    if (aprs_demodulator_.parseExperimentalPacket(experimental_packet)) {
      aprs_experimental_.emplace_back(aprs_demodulator_.frame_,
                                      experimental_packet);
      stats_.total_experimental_packets++;
    } else {
      stats_.num_experimental_packets_failed++;
      other_aprs_packets_.push_back(aprs_demodulator_.frame_);
      stats_.total_other_packets++;
    }
    break;
  }
  case aprs::Packet::Type::TELEMETRY_DATA_REPORT:
  case aprs::Packet::Type::TELEMETRY_COEFFICIENT:
  case aprs::Packet::Type::TELEMETRY_PARAMETER_NAME:
  case aprs::Packet::Type::TELEMETRY_PARAMETER_UNIT:
  case aprs::Packet::Type::TELEMETRY_BIT_SENSE_PROJ_NAME: {
    aprs::TelemetryPacket telemetry_packet;
    if (aprs_demodulator_.parseTelemetryPacket(telemetry_packet)) {
      aprs_telemetry_.emplace_back(aprs_demodulator_.frame_, telemetry_packet);
      stats_.total_telemetry_packets++;
    } else {
      stats_.num_telemetry_packets_failed++;
      other_aprs_packets_.push_back(aprs_demodulator_.frame_);
      stats_.total_other_packets++;
    }
    break;
  }
  default:
    other_aprs_packets_.push_back(aprs_demodulator_.frame_);
    stats_.total_other_packets++;
    break;
  }
}

void Receiver::decode() {
  demodulation_res_ = demodulator_.processAudioBuffer();

  // NRZI-decode the bit stream once, then repeatedly extract frames from
  // the resulting stream. This allows multiple AX.25 frames that were
  // captured in a single receive buffer (back-to-back packets) to all be
  // decoded, instead of only the first one.
  aprs_demodulator_.output_bit_stream_ = demodulator_.output_bit_stream_;
  BitStream nrzi_stream =
      ax25::decodeNrzi(aprs_demodulator_.output_bit_stream_);

  int frame_count = 0;
  while (nrzi_stream.getBitStreamLength() > 0) {
    const int bits_before = nrzi_stream.getBitStreamLength();
    bool res = false;
    try {
      res = aprs_demodulator_.lookForNextAx25Packet(nrzi_stream);
    } catch (...) {
      if (nrzi_stream.getBitStreamLength() == bits_before) {
        break; // no progress; avoid infinite loop
      }
      continue;
    }
    if (nrzi_stream.getBitStreamLength() == bits_before) {
      break; // no progress; avoid infinite loop
    }
    if (res) {
      frame_count++;
      processDecodedFrame();
    }
    // if !res but bits were consumed, a false-positive flag was rejected;
    // keep scanning for the next real flag
  }

  constexpr size_t MAX_FRAMES = 10;
  if (aprs_messages_.size() > MAX_FRAMES) {
    throw Exception(Exception::Id::APRS_RECEIVER_BUFFER_FULL);
  }

  stats_.current_message_packets_in_queue = aprs_messages_.size();
  stats_.current_position_packets_in_queue = aprs_positions_.size();
  stats_.current_experimental_packets_in_queue = aprs_experimental_.size();
  stats_.current_telemetry_packets_in_queue = aprs_telemetry_.size();
  stats_.current_other_packets_in_queue = other_aprs_packets_.size();
}

} // namespace signal_easel::aprs