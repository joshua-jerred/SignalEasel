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

bool afsk::Receiver::process() {
  if (!pulse_audio_reader_) {
    pulse_audio_reader_ = std::make_unique<PulseAudioReader>();
  }
  if (!pulse_audio_reader_->process()) {
    return false;
  }
  detectSignal(pulse_audio_reader_->getAudioBuffer());
  return true;
}

bool afsk::Receiver::detectSignal(const PulseAudioBuffer &audio_buffer) {
  demodulator_.audio_buffer_.clear();
  demodulator_.audio_buffer_ =
      std::vector<int16_t>(audio_buffer.begin(), audio_buffer.end());

  afsk::Demodulator::ProcessResults results{};
  demodulator_.audioBufferToBaseBandSignal(results);

  const bool signal_detected = results.snr > AFSK_SNR_THRESHOLD;

  if (signal_detected) {
    receive_buffer_.insert(receive_buffer_.end(), audio_buffer.begin(),
                           audio_buffer.end());

    // If the signal is sustained (e.g. back-to-back packets with no gap),
    // periodically decode so that the receive buffer doesn't grow without
    // bound and so that packets are delivered in a timely fashion. After
    // decoding, keep a tail of audio samples so that a packet straddling
    // the decode boundary can still be recovered on the next decode.
    if (receive_buffer_.size() >= PERIODIC_DECODE_SAMPLE_COUNT) {
      demodulator_.audio_buffer_ = receive_buffer_;
      decode();
      retainTailSamples();
    }
  } else if (receive_buffer_.size() > AFSK_RECEIVED_MIN_SAMPLES) {
    // Signal dropped after we had been receiving: treat as end-of-burst and
    // decode everything we've accumulated.
    receive_buffer_.insert(receive_buffer_.end(), audio_buffer.begin(),
                           audio_buffer.end());
    demodulator_.audio_buffer_ = receive_buffer_;
    decode();
    receive_buffer_.clear();
  } else {
    // Did not receive a signal, and no meaningful buffer to flush.
    receive_buffer_.clear();
  }

  return signal_detected;
}

void afsk::Receiver::retainTailSamples() {
  if (receive_buffer_.size() <= DECODE_TAIL_SAMPLE_COUNT) {
    receive_buffer_.clear();
    return;
  }
  const size_t drop_count = receive_buffer_.size() - DECODE_TAIL_SAMPLE_COUNT;
  receive_buffer_.erase(receive_buffer_.begin(),
                        receive_buffer_.begin() + drop_count);
}

void afsk::Receiver::decode() {
  demodulator_.processAudioBuffer();

  std::string out_str;
  demodulator_.lookForString(out_str);
}

} // namespace signal_easel