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
std::vector<int16_t> bandPassFilter(std::vector<int16_t> &input,
                                    size_t sample_rate, size_t lower_cutoff,
                                    size_t upper_cutoff);
} // namespace signal_easel

#endif /* SIGNAL_EASEL_FILTER_HPP_ */