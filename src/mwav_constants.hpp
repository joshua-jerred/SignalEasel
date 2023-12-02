/**
 * @file mwav_constants.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Constants used throughout the project
 * @date 2023-12-01
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_CONSTANTS_HPP_
#define MWAV_CONSTANTS_HPP_

#include <cstdint>

namespace mwav {
inline constexpr uint32_t AUDIO_SAMPLE_RATE = 48000;

inline constexpr uint32_t AFSK_BAUD_RATE = 1200;
inline constexpr uint32_t AFSK_MARK_FREQUENCY = 1200;
inline constexpr uint32_t AFSK_SPACE_FREQUENCY = 2200;
inline constexpr uint32_t AFSK_SAMPLES_PER_SYMBOL =
    AUDIO_SAMPLE_RATE / AFSK_BAUD_RATE;

inline constexpr double PI = 3.14159265358979323846;
inline constexpr double TWO_PI = 2.0 * PI;

/// @brief 2^15 (Signed 16-bit PCM)
inline constexpr double MAX_SAMPLE_VALUE = 32768.0;

} // namespace mwav

#endif /* MWAV_CONSTANTS_HPP_ */