/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   pulse_reader.hpp
 * @date   2023-12-10
 * @brief  Reads audio from pulse audio
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_PULSE_AUDIO_HPP_
#define SIGNAL_EASEL_PULSE_AUDIO_HPP_

#include <array>
#include <string>

#include <SignalEasel/constants.hpp>

#include <pulse/error.h>
#include <pulse/simple.h>

namespace signal_easel {

inline constexpr pa_sample_format_t PULSE_AUDIO_SAMPLE_FORMAT = PA_SAMPLE_S16NE;
inline constexpr uint8_t NUM_PULSE_CHANNELS = 1;
inline constexpr pa_sample_spec PULSE_AUDIO_SAMPLE_SPEC = {
    PULSE_AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_RATE, NUM_PULSE_CHANNELS};

const std::string PULSE_AUDIO_APP_NAME = "SignalEasel";

inline constexpr size_t BUFFER_SIZE_IN_SECONDS = 1;
inline constexpr size_t PULSE_AUDIO_BUFFER_SIZE = 200;
// AUDIO_SAMPLE_RATE * NUM_PULSE_CHANNELS * BUFFER_SIZE_IN_SECONDS;

typedef std::array<int16_t, PULSE_AUDIO_BUFFER_SIZE> PulseAudioBuffer;

class PulseAudioReader {
public:
  PulseAudioReader();
  ~PulseAudioReader();

  // rule of 5
  PulseAudioReader(const PulseAudioReader &) = delete;
  PulseAudioReader &operator=(const PulseAudioReader &) = delete;
  PulseAudioReader(PulseAudioReader &&) = delete;
  PulseAudioReader &operator=(PulseAudioReader &&) = delete;

  void process();
  uint32_t getRms() const { return rms_; }
  double getVolume() const { return volume_; }
  uint64_t getLatency() const { return latency_; }

  const PulseAudioBuffer &getAudioBuffer() const { return audio_buffer_; }

private:
  /**
   * @brief Latency in milliseconds
   */
  uint32_t latency_ = 0;
  uint64_t rms_ = 0;
  double volume_ = 0;
  pa_sample_spec ss_ = {PULSE_AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_RATE,
                        NUM_PULSE_CHANNELS};

  pa_simple *s_ = nullptr;

  std::array<int16_t, PULSE_AUDIO_BUFFER_SIZE> audio_buffer_{};
};

} // namespace signal_easel

#endif /* PULSE_READER_HPP_ */