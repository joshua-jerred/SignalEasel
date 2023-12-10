/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   utilities.hpp
 * @date   2023-12-10
 * @brief  Common utilities for SignalEasel
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_UTILITIES_HPP_
#define SIGNAL_EASEL_UTILITIES_HPP_

#include <string>

namespace signal_easel {

/**
 * @brief Basic checks to see if a call sign is valid.
 * @param call_sign The call sign to check
 * @return true if the call sign is valid, false otherwise
 */
bool isCallSignValid(const std::string &call_sign);

} // namespace signal_easel

#endif /* SIGNAL_EASEL_UTILITIES_HPP_ */