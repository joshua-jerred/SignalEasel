/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   afsk_modulator.cpp
 * @date   2023-12-04
 * @brief  The AFSK modulator class implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <SignalEasel/afsk.hpp>

namespace signal_easel {
bool AfskModulator::addBytes(const std::vector<uint8_t> &data) { return true; }
bool AfskModulator::addString(const std::string &data) { return true; }
} // namespace signal_easel