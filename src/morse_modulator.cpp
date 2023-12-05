/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   morse_modulator.cpp
 * @date   2023-12-05
 * @brief  Simple morse code modulator
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <array>
#include <string>

#include <SignalEasel/signal_easel.hpp>

namespace signal_easel {

inline constexpr uint16_t MORSE_TONE_FREQUENCY = 1000;

/**
 * @details
 * §97-119:
 *  (b) The call sign must be transmitted with an emission authorized for the
 *      transmitting channel in one of the following ways:
 *  (1) By a CW emission. When keyed by an automatic device
 *      used only for identification, the speed must not exceed 20 words per
 *      minute.
 */
// Following are in seconds

/**
 * @brief The length of a morse dot in seconds. 0.06 is the minimum for 20wpm
 */
inline constexpr uint16_t DOT_LENGTH = 0.06 * AUDIO_SAMPLE_RATE;
inline constexpr uint16_t DASH_LENGTH = DOT_LENGTH * 3;
inline constexpr uint16_t LETTER_SPACE_LENGTH = DOT_LENGTH * 3;
inline constexpr uint16_t TIME_BETWEEN_SYMBOLS = DOT_LENGTH;

struct MorseChar { // 0 - 9, a - z
  int val;
  const char *morse;
};

static const std::array<std::string, 36> MORSE_CODE_MAP = {
    "-----", // 0
    ".----", // 1
    "..---", // 2
    "...--", // 3
    "....-", // 4
    ".....", // 5
    "-....", // 6
    "--...", // 7
    "---..", // 8
    "----.", // 9
    ".-",    // a
    "-...",  // b
    "-.-.",  // c
    "-..",   // d
    ".",     // e
    "..-.",  // f
    "--.",   // g
    "....",  // h
    "..",    // i
    ".---",  // j
    "-.-",   // k
    ".-..",  // l
    "--",    // m
    "-.",    // n
    "---",   // o
    ".--.",  // p
    "--.-",  // q
    ".-.",   // r
    "...",   // s
    "-",     // t
    "..-",   // u
    "..-",   // v
    ".--",   // w
    "-..-",  // x
    "-.--",  // y
    "--..",  // z
};

void Modulator::addMorseCode(const std::string &input_string) {
  if (input_string.empty()) {
    throw Exception(Exception::Id::EMPTY_INPUT_STRING);
  }

  std::string morse_output;

  // Convert callsign to lowercase and convert to morse
  for (char input_char : input_string) {
    if (input_char >= '0' && input_char <= '9') {
      morse_output += MORSE_CODE_MAP.at(input_char - '0');
    } else if (input_char >= 'a' && input_char <= 'z') {
      morse_output += MORSE_CODE_MAP.at(input_char - 'a' + 10);
    } else if (input_char >= 'A' && input_char <= 'Z') {
      morse_output += MORSE_CODE_MAP.at(input_char - 'A' + 10);
    } else {
      throw Exception(Exception::Id::INVALID_MORSE_CHAR);
    }
    morse_output += " ";
  }

  // Add morse callsign to wavgen
  for (char c : morse_output) {
    if (c == '.') {
      addSineWave(MORSE_TONE_FREQUENCY, DOT_LENGTH);
    } else if (c == '-') {
      addSineWave(MORSE_TONE_FREQUENCY, DASH_LENGTH);
    } else if (c == ' ') {
      addSilence(LETTER_SPACE_LENGTH);
      continue;
    }
    addSilence(TIME_BETWEEN_SYMBOLS);
  }
}

} // namespace signal_easel