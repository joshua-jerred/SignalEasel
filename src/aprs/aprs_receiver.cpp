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

bool Receiver::getOtherAprsPacket(ax25::Frame &frame) {
  if (other_aprs_packets_.empty()) {
    return false;
  }
  frame = other_aprs_packets_.back();
  other_aprs_packets_.pop_back();
  return true;
}

void Receiver::decode() {
  demodulation_res_ = demodulator_.processAudioBuffer();

  // std::cout << "Decoding [";
  // std::cout << std::setprecision(2) << std::fixed
  //           << "SNR: " << demodulation_res.snr << "]" << std::endl;

  aprs_demodulator_.output_bit_stream_ = demodulator_.output_bit_stream_;
  bool res = false;
  try {
    res = aprs_demodulator_.lookForAx25Packet();
  } catch (Exception &e) {
    // std::cout << "Exception: " << e.what() << std::endl;
    return;
  }

  const auto type = aprs_demodulator_.getType();

  if (res && type == aprs::Packet::Type::MESSAGE) {

    aprs::MessagePacket message_packet;
    bool success = aprs_demodulator_.parseMessagePacket(message_packet);
    if (success) {
      aprs_messages_.push_back(std::pair<ax25::Frame, aprs::MessagePacket>(
          aprs_demodulator_.frame_, message_packet));
      stats_.total_message_packets++;
    } else {
      stats_.num_message_packets_failed++;
    }

  } else if (res && type == aprs::Packet::Type::POSITION) {

    aprs::PositionPacket position_packet;
    bool success = aprs_demodulator_.parsePositionPacket(position_packet);
    if (success) {
      position_packet.decoded_timestamp.setToNow();
      aprs_positions_.push_back(std::pair<ax25::Frame, aprs::PositionPacket>(
          aprs_demodulator_.frame_, position_packet));
      stats_.total_position_packets++;
    } else {
      stats_.num_position_packets_failed++;
    }

  } else if (res && type == aprs::Packet::Type::EXPERIMENTAL) {

    aprs::ExperimentalPacket experimental_packet;
    bool success =
        aprs_demodulator_.parseExperimentalPacket(experimental_packet);
    if (success) {
      aprs_experimental_.push_back(
          std::pair<ax25::Frame, aprs::ExperimentalPacket>(
              aprs_demodulator_.frame_, experimental_packet));
      stats_.total_experimental_packets++;
    } else {
      stats_.num_experimental_packets_failed++;
    }

  } else if (res &&
             (type == aprs::Packet::Type::TELEMETRY_DATA_REPORT ||
              type == aprs::Packet::Type::TELEMETRY_COEFFICIENT ||
              type == aprs::Packet::Type::TELEMETRY_PARAMETER_NAME ||
              type == aprs::Packet::Type::TELEMETRY_PARAMETER_UNIT ||
              type == aprs::Packet::Type::TELEMETRY_BIT_SENSE_PROJ_NAME)) {

    aprs::TelemetryPacket telemetry_packet;
    bool success = aprs_demodulator_.parseTelemetryPacket(telemetry_packet);
    if (success) {
      aprs_telemetry_.push_back(std::pair<ax25::Frame, aprs::TelemetryPacket>(
          aprs_demodulator_.frame_, telemetry_packet));
      stats_.total_telemetry_packets++;
    } else {
      stats_.num_telemetry_packets_failed++;
    }

  } else if (res) {
    other_aprs_packets_.push_back(aprs_demodulator_.frame_);
    stats_.total_other_packets++;
  } else {
    // std::cout << "No packet found" << std::endl;
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