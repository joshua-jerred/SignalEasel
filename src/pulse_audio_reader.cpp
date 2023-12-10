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

  for (int16_t sample : audio_buffer_) {
    std::cout << sample << std::endl;
  }
}

} // namespace signal_easel

#endif // PULSE_AUDIO_ENABLED