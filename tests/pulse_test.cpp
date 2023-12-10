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

#include <cmath>
#include <iostream>

void printVolumeBar(double volume) {
  std::cout << "\r";
  for (int i = 0; i < 50; i++) {
    if (i < volume * 50) {
      std::cout << "#";
    } else {
      std::cout << " ";
    }
  }
  std::cout << " " << volume;
  std::cout.flush();
}

int main() {
  signal_easel::AfskModulator modulator;
  modulator.addString("Hello World!");
  modulator.writeToPulseAudio();

  signal_easel::PulseAudioReader reader;

  while (true) {
    reader.process();
    printVolumeBar(reader.getVolume());
  }

  return 0;
}