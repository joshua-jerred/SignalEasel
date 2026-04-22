/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   receiver.hpp
 * @date   2023-12-10
 * @brief  Base class for receivers. Demodulators that use pulse audio.
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_RECEIVER_HPP_
#define SIGNAL_EASEL_RECEIVER_HPP_

#include <memory>

#include <SignalEasel/pulse_audio.hpp>
#include <SignalEasel/settings.hpp>

namespace signal_easel {

class Receiver {
public:
  Receiver(Settings settings = Settings()) : settings_(std::move(settings)) {}
  virtual ~Receiver() = default;

  virtual bool process() = 0;

  uint64_t getLatency() const {
    return pulse_audio_reader_ ? pulse_audio_reader_->getLatency() : 0;
  }

  double getVolume() const {
    return pulse_audio_reader_ ? pulse_audio_reader_->getVolume() : 0.0;
  }

protected:
  /// @brief Lazily constructed on first process() call so that tests can
  /// override process() without triggering a real PulseAudio connection.
  std::unique_ptr<PulseAudioReader> pulse_audio_reader_{};
  Settings settings_;
};

} // namespace signal_easel

#endif /* SIGNAL_EASEL_RECEIVER_HPP_ */