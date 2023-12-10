/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   demodulator.hpp
 * @date   2023-12-10
 * @brief  Base class for demodulators
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_DEMODULATOR_HPP_
#define SIGNAL_EASEL_DEMODULATOR_HPP_

#include <string>
#include <vector>

#include <SignalEasel/settings.hpp>

namespace signal_easel {

/**
 * @brief The base class for all demodulators that turn audio into data
 */
class Demodulator {
public:
  Demodulator(Settings settings) : settings_(std::move(settings)) {}
  virtual ~Demodulator() = default;

  /**
   * @brief Clears the audio buffer
   */
  void clearBuffer() { audio_buffer_.clear(); }

  /**
   * @brief Open a WAV file and load it's contents into the audio buffer.
   * @param file_name The name/path of the file to open
   */
  void loadAudioFromFile(const std::string &file_name);

#ifdef PULSE_AUDIO_ENABLED
  // virtual bool detectSignal() = 0;
  // virtual void processPulseAudio() = 0;
#endif

protected:
  Settings settings_;
  std::vector<int16_t> audio_buffer_ = {};

  const std::vector<int16_t> &getAudioBuffer() const { return audio_buffer_; }
};

} // namespace signal_easel

#endif /* SIGNAL_EASEL_DEMODULATOR_HPP_ */