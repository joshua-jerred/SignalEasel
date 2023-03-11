#include <iostream> // DEBUG
#include <string>
#include <sstream>

#include "modulators.h"

#define AUDIO_FREQ 1000 // Hz
#define AUDIO_AMP 0.5 // 0.0 - 1.0

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
#define DOT_LENGTH 0.06 // 0.06 is the minimum for 20wpm
#define DASH_LENGTH (DOT_LENGTH * 3.0)
#define LETTER_SPACE_LENGTH (DOT_LENGTH * 3.0)
#define TIME_BETWEEN_SYMBOLS (DOT_LENGTH)

struct MorseChar { // 0 - 9, a - z
    int val;
    const char* morse;
};

const MorseChar morse_chars[] = {
    {0,  "-----"}, // 4
    {1,  ".----"}, // 0
    {2,  "..---"}, // 1
    {3,  "...--"}, // 2
    {4,  "....-"}, // 3
    {5,  "....."}, // 5
    {6,  "-...."}, // 6
    {7,  "--..."}, // 7
    {8,  "---.."}, // 8
    {9,  "----."}, // 9
    {10, ".-"   }, // a
    {11, "-..." }, // b
    {12, "-.-." }, // c
    {13, "-.."  }, // d
    {14, "."    }, // e
    {15, "..-." }, // f
    {16, "--."  }, // g
    {17, "...." }, // h
    {18, ".."   }, // i
    {19, ".---" }, // j
    {20, "-.-"  }, // k
    {21, ".-.." }, // l
    {22, "--"   }, // m
    {23, "-."   }, // n
    {24, "---"  }, // o
    {25, ".--." }, // p
    {26, "--.-" }, // q
    {27, ".-."  }, // r
    {28, "..."  }, // s
    {29, "-"    }, // t
    {30, "..-"  }, // u
    {31, "..-"  }, // v
    {32, ".--"  }, // w
    {33, "-..-" }, // x
    {34, "-.--" }, // y
    {35, "--.." }, // z
};



bool modulators::EncodeMorse(WavGen &wavgen, std::string callsign) {
    if (callsign.size() < 4 || callsign.size() > 6) {
        return false;
    }
    std::string morse_callsign = "";
    // Convert callsign to lowercase and convert to morse
    for (int i = 0; i < (int) callsign.size(); i++) {
        callsign[i] = tolower(callsign[i]);
        char c = callsign[i];
        if (c >= '0' && c <= '9') {
            morse_callsign += morse_chars[c - '0'].morse;
        } else if (c >= 'a' && c <= 'z') {
            morse_callsign += morse_chars[c - 'a' + 10].morse;
        } else {
            std::cout << "Invalid character in callsign: " << c << std::endl; // DEBUG
            return false;
        }
        morse_callsign += " ";
    }

    // Add morse callsign to wavgen
    for (int i = 0; i < (int) morse_callsign.size(); i++) {
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
    std::cout << "Callsign: " << callsign << std::endl;
    std::cout << "Morse: " << morse_callsign << std::endl;
    return true;
}