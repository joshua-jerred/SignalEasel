/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   modulator.cpp
 * @date   2023-12-04
 * @brief  The abstract modulator class implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/signal_easel.hpp>

namespace signal_easel {
void Modulator::writeToFile(const std::string &filename) { (void)filename; }

void DataModulator::addBytes(const std::vector<uint8_t> &data) {
  if (data.empty()) {
    return;
  }
  encodeBytes(data);
}

void DataModulator::addString(const std::string &data) {
  if (data.empty()) {
    return;
  }
  encodeBytes(std::vector<uint8_t>(data.begin(), data.end()));
}

} // namespace signal_easel