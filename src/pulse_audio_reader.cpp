/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   pulse_audio_reader.cpp
 * @date   2023-12-10
 * @brief  PulseAudio reader implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include "pulse_audio.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#ifdef PULSE_AUDIO_ENABLED

namespace signal_easel {

PulseAudioReader::PulseAudioReader() {
  s_ = pa_simple_new(nullptr,                  // Use the default server.
                     PULSE_AUDIO_APP_NAME,     // Our application's name.
                     PA_STREAM_RECORD,         // Stream direction (output).
                     nullptr,                  // Use the default device.
                     "StreamReader",           // Description of our stream.
                     &PULSE_AUDIO_SAMPLE_SPEC, // The output sample format
                     nullptr,                  // Use default channel map
                     nullptr, // Use default buffering attributes.
                     nullptr  // Ignore error code.
  );

  if (!s_) {
    throw Exception(Exception::Id::PULSE_OPEN_ERROR);
  }
}

PulseAudioReader::~PulseAudioReader() { pa_simple_free(s_); }

void PulseAudioReader::process() {
  int error = 0;
  // clang-format off
  if (pa_simple_read(
      s_,                   // The stream to read from
      audio_buffer_.data(), // Where to store the read data
      static_cast<size_t>(audio_buffer_.size() * sizeof(int16_t)), // How much to read
      &error // Ignore error code
  ) < 0) {
    // clang-format on
    throw Exception(Exception::Id::PULSE_READ_ERROR);
  }

  rms_ = 1;
  for (int16_t sample : audio_buffer_) {
    rms_ += static_cast<int64_t>(sample) * static_cast<int64_t>(sample);
  }
  rms_ = std::sqrt(rms_ / audio_buffer_.size());
  volume_ = (static_cast<double>(rms_) / 32768.0) / 0.72;
  if (volume_ < 0.01) {
    volume_ = 0;
  }
}

} // namespace signal_easel

#endif // PULSE_AUDIO_ENABLED