/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   bp_filter.hpp
 * @date   2023-12-05
 * @brief  Bandpass filter implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef BP_FILTER_HPP_
#define BP_FILTER_HPP_

#include <cstdint>
#include <vector>

namespace signal_easel {

/**
 * @brief A band-pass filter implementation
 *
 * @param input The input signal
 * @param sample_rate The sample rate of the input signal (ie. 44100)
 * @param lower_cutoff The lower cutoff frequency
 * @param upper_cutoff The upper cutoff frequency
 * @param filter_order The order of the filter (ie. 4)
 * @return std::vector<double> The filtered signal
 */
std::vector<double> bandPassFilter(const std::vector<double> &input,
                                   double sample_rate, double lower_cutoff,
                                   double upper_cutoff,
                                   size_t filter_order = 4);
} // namespace signal_easel

#endif /* SIGNAL_EASEL_FILTER_HPP_ */