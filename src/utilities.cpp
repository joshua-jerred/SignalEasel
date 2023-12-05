/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   utilities.cpp
 * @date   2023-12-05
 * @brief  Some utility functions for the library
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <algorithm>

#include <SignalEasel/signal_easel.hpp>

namespace signal_easel {
bool isCallSignValid(const std::string &call_sign) {
  constexpr size_t k_min_call_sign_length = 3;
  constexpr size_t k_max_call_sign_length = 6;

  if (call_sign.length() < k_min_call_sign_length ||
      call_sign.length() > k_max_call_sign_length) {
    return false;
  }

  if (std::all_of(call_sign.begin(), call_sign.end(),
                  [](char symbol) { return std::isdigit(symbol); })) {
    return false;
  }

  return true;
}
} // namespace signal_easel