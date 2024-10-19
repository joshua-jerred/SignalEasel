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
#include <cctype>

#include "utilities.hpp"

namespace signal_easel {

bool isCallSignValid(const std::string &call_sign) {
  constexpr size_t k_min_call_sign_length = 3;
  constexpr size_t k_max_call_sign_length = 7;

  if (call_sign.length() < k_min_call_sign_length ||
      call_sign.length() > k_max_call_sign_length) {
    return false;
  }

  bool contains_digit = false;
  bool contains_letter = false;
  for (const auto &symbol : call_sign) {
    if (std::isdigit(symbol)) { // Check if there is at least one digit
      contains_digit = true;
    } else if (std::isalpha(symbol)) { // Check if there is at least one letter
      contains_letter = true;
    } else {
      return false; // non-alphanumeric character
    }
  }

  return contains_digit && contains_letter;
}

} // namespace signal_easel