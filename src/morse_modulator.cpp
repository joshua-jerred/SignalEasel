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
inline constexpr double DOT_LENGTH = 0.06;
inline constexpr double DASH_LENGTH = DOT_LENGTH * 3;
inline constexpr double LETTER_SPACE_LENGTH = DOT_LENGTH * 3;
inline constexpr double TIME_BETWEEN_SYMBOLS = DOT_LENGTH;

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

bool encodeMorse(WavGen &wavgen, std::string callsign) {
  std::string morse_callsign = "";
  // Convert callsign to lowercase and convert to morse
  for (int i = 0; i < (int)callsign.size(); i++) {
    callsign[i] = tolower(callsign[i]);
    char c = callsign[i];
    if (c >= '0' && c <= '9') {
      morse_callsign += morse_chars[c - '0'].morse;
    } else if (c >= 'a' && c <= 'z') {
      morse_callsign += morse_chars[c - 'a' + 10].morse;
    } else {
      throw mwav::Exception("Invalid character in callsign");
    }
    morse_callsign += " ";
  }

  // Add morse callsign to wavgen
  for (int i = 0; i < (int)morse_callsign.size(); i++) {
    char c = morse_callsign[i];
    if (c == '.') {
      wavgen.addSineWave(AUDIO_FREQ, AUDIO_AMP, DOT_LENGTH);
    } else if (c == '-') {
      wavgen.addSineWave(AUDIO_FREQ, AUDIO_AMP, DASH_LENGTH);
    } else if (c == ' ') {
      wavgen.addSineWave(0, 0, LETTER_SPACE_LENGTH);
      continue;
    }
    wavgen.addSineWave(0, 0, TIME_BETWEEN_SYMBOLS);
  }

  // DEBUG
  // std::cout << "Callsign: " << callsign << std::endl;
  // std::cout << "Morse: " << morse_callsign << std::endl;
  return true;
}

} // namespace signal_easel