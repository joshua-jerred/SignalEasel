/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   afsk_receiver.cpp
 * @date   2023-12-10
 * @brief  Implementation of the AFSK receiver class.
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/afsk.hpp>

#include <iomanip>
#include <iostream>

namespace signal_easel {

void afsk::Receiver::process() {
  pulse_audio_reader_.process();
  detectSignal(pulse_audio_reader_.getAudioBuffer());
}

bool afsk::Receiver::detectSignal(const PulseAudioBuffer &audio_buffer) {
  demodulator_.audio_buffer_.clear();
  demodulator_.audio_buffer_ =
      std::vector<int16_t>(audio_buffer.begin(), audio_buffer.end());

  afsk::Demodulator::ProcessResults results{};
  demodulator_.audioBufferToBaseBandSignal(results);

  bool signal_detected = false;
  if (results.snr > AFSK_SNR_THRESHOLD) {
    signal_detected = true;
  }

  if (signal_detected) {
    receive_buffer_.insert(receive_buffer_.end(), audio_buffer.begin(),
                           audio_buffer.end());
  } else if (receive_buffer_.size() > AFSK_RECEIVED_MIN_SAMPLES) {
    receive_buffer_.insert(receive_buffer_.end(), audio_buffer.begin(),
                           audio_buffer.end());
    demodulator_.audio_buffer_ = receive_buffer_;
    decode();
    receive_buffer_.clear();
  } else {
    // Did not receive a signal, but we should keep the last few samples
    // in case the signal comes back.
    receive_buffer_.clear();
    // receive_buffer_ =
    // std::vector<int16_t>(audio_buffer.begin(), audio_buffer.end());
  }

  return true;
}

void afsk::Receiver::decode() {
  auto demodulation_res = demodulator_.processAudioBuffer();

  std::cout << "Decoding [";
  std::cout << std::setprecision(2) << std::fixed
            << "SNR: " << demodulation_res.snr << "]: ";

  std::string out_str;
  auto str_res = demodulator_.lookForString(out_str);

  if (str_res == afsk::Demodulator::AsciiResult::SUCCESS) {
    std::cout << out_str << std::endl;
  } else if (str_res == afsk::Demodulator::AsciiResult::NO_SYN) {
    // std::cout << "No SYN" << std::endl;
  } else if (str_res == afsk::Demodulator::AsciiResult::NO_EOT) {
    // std::cout << "No EOT" << std::endl;
  } else {
    std::cout << "Unknown" << std::endl;
  }
}

} // namespace signal_easel