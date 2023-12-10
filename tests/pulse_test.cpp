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

#include <SignalEasel/afsk.hpp>
#include <SignalEasel/aprs.hpp>

#define PULSE_AUDIO_ENABLED
#include <SignalEasel/pulse_audio.hpp>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <unistd.h>

void printVolumeBar(double volume, uint16_t latency) {
  std::cout << "\r";
  for (int i = 0; i < 50; i++) {
    if (i < volume * 50) {
      std::cout << "#";
    } else {
      std::cout << " ";
    }
  }
  std::cout << " " << std::setprecision(2) << std::fixed << volume << " | "
            << latency << "ms"
            << "         ";
  std::cout.flush();
}

int main() {
  signal_easel::AfskModulator modulator;
  modulator.addString("Hello World!");
  modulator.writeToPulseAudio();

  signal_easel::AprsReceiver receiver;

  while (true) {
    receiver.process();
    // reader.process();
    // printVolumeBar(reader.getVolume(), reader.getLatency());
  }

  return 0;
}