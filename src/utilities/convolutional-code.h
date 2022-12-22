#ifndef CONVOLUTIONAL_CODE_H_
#define CONVOLUTIONAL_CODE_H_

#include <map>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/**
 * @brief Convolutional Code. Left shifting 5 bit values mapped to their
 * respective phase shift values in degrees. Supports all binary values
 * 0x00 to 0x1F.
 * @cite http://www.arrl.org/psk31-spec
 */
std::map<unsigned char, double> conv_code = {
    {0b00000, M_PI},
    {0b00001, M_PI/2.0 },
    {0b00010, -(M_PI/2.0)},
    {0b00011, 0  },
    {0b00100, -(M_PI/2.0)},
    {0b00101, 0  },
    {0b00110, M_PI},
    {0b00111, M_PI/2.0 },
    {0b01000, 0  },
    {0b01001, -(M_PI/2.0)},
    {0b01010, M_PI/2.0 },
    {0b01011, M_PI},
    {0b01100, M_PI/2.0 },
    {0b01101, M_PI},
    {0b01110, 0 },
    {0b01111, -(M_PI/2.0)},
    {0b10000, M_PI/2.0 },
    {0b10001, M_PI},
    {0b10010, 0  },
    {0b10011, -(M_PI/2.0)},
    {0b10100, 0  },
    {0b10101, -(M_PI/2.0)},
    {0b10110, M_PI/2.0 },
    {0b10111, M_PI},
    {0b11000, -(M_PI/2.0)},
    {0b11001, 0  },
    {0b11010, M_PI},
    {0b11011, M_PI/2.0 },
    {0b11100, M_PI},
    {0b11101, M_PI/2.0 },
    {0b11110, -(M_PI/2.0)},
    {0b11111, 0  }
};
#endif // CONVOLUTIONAL_CODE_H_