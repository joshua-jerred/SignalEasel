/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   pulse_test.cpp
 * @date   2023-12-09
 * @brief  PulseAudio output test. Does not use gtest.
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/aprs.hpp>

#define PULSE_AUDIO_ENABLED
#include "../src/pulse_audio.hpp"

int main() {
  signal_easel::AfskModulator modulator;
  modulator.addString("Hello World!");
  modulator.writeToPulseAudio();

  signal_easel::PulseAudioReader reader;

  while (true)
    reader.process();

  return 0;
}