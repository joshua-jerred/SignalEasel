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
  signal_easel::afsk::Modulator modulator;
  modulator.addString("Hello World!");
  modulator.writeToPulseAudio();

  signal_easel::aprs::Receiver receiver;

  while (true) {
    if (receiver.process()) {
      std::cout << "Last Packet / Live SNR: " << receiver.getSNR() << "dB"
                << " " << receiver.getLiveSnr() << "dB" << std::endl;
      auto stats = receiver.getStats();
      std::cout << "Total message packets: " << stats.total_message_packets
                << std::endl;
      std::cout << "Total position packets: " << stats.total_position_packets
                << std::endl;
      std::cout << "Total experimental packets: "
                << stats.total_experimental_packets << std::endl;
      std::cout << "Total telemetry packets: " << stats.total_telemetry_packets
                << std::endl;
      std::cout << "Total other packets: " << stats.total_other_packets
                << std::endl;
    }

    // reader.process();
    printVolumeBar(receiver.getVolume(), receiver.getLatency());
  }

  return 0;
}