/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   pulse_audio_writer.cpp
 * @date   2023-12-09
 * @brief  Simple pulse audio interface
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/exception.hpp>
#include <SignalEasel/modulator.hpp>
#include <SignalEasel/pulse_audio.hpp>

namespace signal_easel {

void Modulator::writeToPulseAudio() {
#ifndef PULSE_AUDIO_ENABLED
  throw Exception(Exception::Id::PULSE_AUDIO_DISABLED);
#else

  // ss.format = PA_SAMPLE_S16LE;
  // ss.channels = 1;
  // ss.rate = AUDIO_SAMPLE_RATE;

  pa_simple *s =
      pa_simple_new(nullptr,                      // Use the default server.
                    PULSE_AUDIO_APP_NAME.c_str(), // Our application's name.
                    PA_STREAM_PLAYBACK,           // Stream direction (output).
                    nullptr,                      // Use the default device.
                    "StreamWriter",               // Description of our stream.
                    &PULSE_AUDIO_SAMPLE_SPEC,     // The output sample format
                    nullptr,                      // Use default channel map
                    nullptr, // Use default buffering attributes.
                    nullptr  // Ignore error code.
      );

  if (!s) {
    throw Exception(Exception::Id::PULSE_OPEN_ERROR);
  }

  if (pa_simple_write(
          s, audio_buffer_.data(),
          static_cast<size_t>(audio_buffer_.size() * sizeof(int16_t)),
          nullptr) < 0) {
    throw Exception(Exception::Id::PULSE_WRITE_ERROR);
  }

  int res = pa_simple_drain(s, nullptr);
  if (res < 0) {
    throw Exception(Exception::Id::PULSE_DRAIN_ERROR);
  }

  pa_simple_free(s);
#endif
}

} // namespace signal_easel
