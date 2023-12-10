/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   constants.hpp
 * @date   2023-12-10
 * @brief  Common constants used throughout the library
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_CONSTANTS_HPP_
#define SIGNAL_EASEL_CONSTANTS_HPP_

#include <cstdint>

namespace signal_easel {

/**
 * @brief The sample rate of the audio
 * @details This value was chosen as it's cleanly
 * devisable by 1200 and 60, this is helpful for AFSK
 * and SSTV. Choosing the standard 44100 creates
 * problems, especially with SSTV which was created in
 * the days of 60Hz.
 */
inline constexpr uint32_t AUDIO_SAMPLE_RATE = 48000;

/**
 * @brief Just a double version of AUDIO_SAMPLE_RATE. I'm tired of casting.
 */
inline constexpr double AUDIO_SAMPLE_RATE_D =
    static_cast<double>(AUDIO_SAMPLE_RATE);

/**
 * @brief The number of bits per sample (16-bit PCM)
 */
inline constexpr uint32_t SAMPLE_RESOLUTION = 16;

/**
 * @brief The maximum value of a sample (2^15 - 1)
 */
inline constexpr uint32_t MAX_SAMPLE_VALUE = 32767;

/**
 * @brief The maximum size of the audio buffer (5 minutes)
 */
inline constexpr uint32_t MAX_AUDIO_BUFFER_SIZE = AUDIO_SAMPLE_RATE * 60 * 5;

/**
 * @brief Constant for PI used throughout the project
 */
inline constexpr double PI_VAL = 3.14159265358979323846;

/**
 * @brief Constant for 2*PI used throughout the project
 */
inline constexpr double TWO_PI_VAL = 2.0 * PI_VAL;

} // namespace signal_easel

#endif /* SIGNAL_EASEL_CONSTANTS_HPP_ */