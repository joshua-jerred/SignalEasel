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

#include <SignalEasel/signal_easel.hpp>

#ifdef PULSE_AUDIO_ENABLED
#include <array>
#include <pulse/error.h>
#include <pulse/simple.h>
#endif

namespace signal_easel {

#ifdef PULSE_AUDIO_ENABLED

inline constexpr pa_sample_format_t PULSE_AUDIO_SAMPLE_FORMAT = PA_SAMPLE_S16LE;
inline constexpr uint8_t NUM_PULSE_CHANNELS = 1;
inline constexpr pa_sample_spec PULSE_AUDIO_SAMPLE_SPEC = {
    PULSE_AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_RATE, NUM_PULSE_CHANNELS};

static const char *PULSE_AUDIO_APP_NAME = "SignalEasel";

inline constexpr size_t BUFFER_SIZE_IN_SECONDS = 1;
inline constexpr size_t PULSE_AUDIO_BUFFER_SIZE = 500;
// AUDIO_SAMPLE_RATE * NUM_PULSE_CHANNELS * BUFFER_SIZE_IN_SECONDS;

#endif

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

private:
#ifdef PULSE_AUDIO_ENABLED
  pa_sample_spec ss_ = {PULSE_AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_RATE,
                        NUM_PULSE_CHANNELS};

  pa_simple *s_ = nullptr;

  std::array<int16_t, PULSE_AUDIO_BUFFER_SIZE> audio_buffer_{};
#endif
};

} // namespace signal_easel

#endif /* PULSE_READER_HPP_ */