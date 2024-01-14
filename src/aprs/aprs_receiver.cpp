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

namespace signal_easel {

void aprs::Receiver::decode() {
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

  if (res == true && type == aprs::Packet::Type::MESSAGE) {
    aprs::MessagePacket message_packet;
    bool success = aprs_demodulator_.parseMessagePacket(message_packet);
    if (success == true) {
      aprs_messages_.push_back(std::pair<ax25::Frame, aprs::MessagePacket>(
          aprs_demodulator_.frame_, message_packet));
      stats_.total_message_packets++;
    } else {
      stats_.num_message_packets_failed++;
    }
  } else if (res == true && type == aprs::Packet::Type::POSITION) {
    aprs::PositionPacket position_packet;
    bool success = aprs_demodulator_.parsePositionPacket(position_packet);
    if (success == true) {
      aprs_positions_.push_back(std::pair<ax25::Frame, aprs::PositionPacket>(
          aprs_demodulator_.frame_, position_packet));
      stats_.total_position_packets++;
    } else {
      stats_.num_position_packets_failed++;
    }
  } else if (res == true) {
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
  stats_.current_other_packets_in_queue = other_aprs_packets_.size();
}

} // namespace signal_easel