/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   settings.hpp
 * @date   2023-12-10
 * @brief  Settings structures for SignalEasel
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_SETTINGS_HPP_
#define SIGNAL_EASEL_SETTINGS_HPP_

#include <string>

namespace signal_easel {

/**
 * @brief The universal settings for MWAV
 */
struct Settings {
  /**
   * @brief Used to determine if a morse call sign should be added to the
   * audio.
   * @details Add a morse call sign before, after, or both before and after
   * the audio.
   */
  enum class CallSignMode { NONE, BEFORE, AFTER, BEFORE_AND_AFTER };

  /**
   * @brief An amateur radio call sign
   * @details Can be left empty if call_sign_mode is set to NONE, but not if
   * using the APRS modulator. Must conform to the FCC's rules for call signs.
   * @warning This may be a problem for international users. If so, please
   * create an issue on GitHub.
   */
  std::string call_sign{};

  /**
   * @brief The mode for the call sign
   * @see CallSignMode
   */
  Settings::CallSignMode call_sign_mode = Settings::CallSignMode::NONE;

  /**
   * @brief The maximum amplitude of the audio. 0.0 - 1.0
   */
  double amplitude = 0.5;

  /**
   * @brief The amount of silence to add between the morse code call sign and
   * any other audio. In seconds.
   */
  double call_sign_pause_seconds = 0.1;
};

} // namespace signal_easel

#endif /* SIGNAL_EASEL_SETTINGS_HPP_ */