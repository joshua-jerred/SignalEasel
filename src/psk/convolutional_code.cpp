/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   convolutional_code.cpp
 * @date   2024-06-08
 * @brief  Convolutional Code Implementation
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <cmath>

#include <SignalEasel/psk.hpp>

namespace signal_easel::psk {

const std::map<uint8_t, double> ConvolutionalCode{{0b00000, M_PI},
                                                  {0b00001, M_PI / 2.0},
                                                  {0b00010, -(M_PI / 2.0)},
                                                  {0b00011, 0},
                                                  {0b00100, -(M_PI / 2.0)},
                                                  {0b00101, 0},
                                                  {0b00110, M_PI},
                                                  {0b00111, M_PI / 2.0},
                                                  {0b01000, 0},
                                                  {0b01001, -(M_PI / 2.0)},
                                                  {0b01010, M_PI / 2.0},
                                                  {0b01011, M_PI},
                                                  {0b01100, M_PI / 2.0},
                                                  {0b01101, M_PI},
                                                  {0b01110, 0},
                                                  {0b01111, -(M_PI / 2.0)},
                                                  {0b10000, M_PI / 2.0},
                                                  {0b10001, M_PI},
                                                  {0b10010, 0},
                                                  {0b10011, -(M_PI / 2.0)},
                                                  {0b10100, 0},
                                                  {0b10101, -(M_PI / 2.0)},
                                                  {0b10110, M_PI / 2.0},
                                                  {0b10111, M_PI},
                                                  {0b11000, -(M_PI / 2.0)},
                                                  {0b11001, 0},
                                                  {0b11010, M_PI},
                                                  {0b11011, M_PI / 2.0},
                                                  {0b11100, M_PI},
                                                  {0b11101, M_PI / 2.0},
                                                  {0b11110, -(M_PI / 2.0)},
                                                  {0b11111, 0}};

} // namespace signal_easel::psk