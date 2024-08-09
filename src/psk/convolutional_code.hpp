/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   convolutional_code.hpp
 * @date   2024-08-08
 * @brief  Convolutional Code Implementation
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#pragma once

#include <cstdint>
#include <map>

#include "psk_wave_shaper.hpp"

namespace signal_easel::psk {

/**
 * @brief Given a five bit value, return the phase shift for the convolutional
 * code.
 * @param code - The five bit value (lower 5 bits)
 * @return double - The phase shift in radians
 */
double getConvolutionalCodeShift(uint8_t code);

/**
 * @brief Shift the provided phase with the convolutional code.
 *
 * @param code - The five bit value (lower 5 bits)
 * @param current_phase - The current phase
 * @return Phase - The current phase shifted by the convolutional code
 */
Phase getShiftedConvolutionalCodePhase(uint8_t code, Phase current_phase);

} // namespace signal_easel::psk