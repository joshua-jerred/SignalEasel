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

#include <SignalEasel/signal_easel.hpp>

namespace signal_easel {
bool isCallSignValid(const std::string &call_sign) {
  constexpr size_t kMinCallSignLength = 3;
  constexpr size_t kMaxCallSignLength = 6;

  if (call_sign.length() < kMinCallSignLength ||
      call_sign.length() > kMaxCallSignLength) {
    return false;
  }

  for (char c : call_sign) {
    if (!std::isalnum(c)) {
      return false;
    }
  }

  return true;
}
} // namespace signal_easel