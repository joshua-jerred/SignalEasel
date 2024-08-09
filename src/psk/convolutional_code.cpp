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

#include <SignalEasel/exception.hpp>
#include <SignalEasel/psk.hpp>
#include <SignalEasel/settings.hpp>

#include "convolutional_code.hpp"

namespace signal_easel::psk {

enum class Shift : uint8_t {
  ZERO = 0,
  PI_OVER_TWO = 1,
  MINUS_PI_OVER_TWO = 2,
  PI = 3
};

static const std::map<uint8_t, Shift> ConvolutionalCode{
    {0b00000, Shift::PI},
    {0b00001, Shift::PI_OVER_TWO},
    {0b00010, Shift::MINUS_PI_OVER_TWO},
    {0b00011, Shift::ZERO},
    {0b00100, Shift::MINUS_PI_OVER_TWO},
    {0b00101, Shift::ZERO},
    {0b00110, Shift::PI},
    {0b00111, Shift::PI_OVER_TWO},
    {0b01000, Shift::ZERO},
    {0b01001, Shift::MINUS_PI_OVER_TWO},
    {0b01010, Shift::PI_OVER_TWO},
    {0b01011, Shift::PI},
    {0b01100, Shift::PI_OVER_TWO},
    {0b01101, Shift::PI},
    {0b01110, Shift::ZERO},
    {0b01111, Shift::MINUS_PI_OVER_TWO},
    {0b10000, Shift::PI_OVER_TWO},
    {0b10001, Shift::PI},
    {0b10010, Shift::ZERO},
    {0b10011, Shift::MINUS_PI_OVER_TWO},
    {0b10100, Shift::ZERO},
    {0b10101, Shift::MINUS_PI_OVER_TWO},
    {0b10110, Shift::PI_OVER_TWO},
    {0b10111, Shift::PI},
    {0b11000, Shift::MINUS_PI_OVER_TWO},
    {0b11001, Shift::ZERO},
    {0b11010, Shift::PI},
    {0b11011, Shift::PI_OVER_TWO},
    {0b11100, Shift::PI},
    {0b11101, Shift::PI_OVER_TWO},
    {0b11110, Shift::MINUS_PI_OVER_TWO},
    {0b11111, Shift::ZERO}};

double getConvolutionalCodeShift(uint8_t code) {
  switch (ConvolutionalCode.at(code)) {
  case Shift::ZERO:
    return 0.0;
  case Shift::PI_OVER_TWO:
    return M_PI / 2;
  case Shift::MINUS_PI_OVER_TWO:
    return -M_PI / 2;
  case Shift::PI:
    return M_PI;
  }

  throw signal_easel::Exception("Invalid Convolutional Code");
}

static const std::map<Shift, const std::map<Phase, Phase>> PhaseShiftMap{
    {Shift::ZERO,
     {{Phase::ZERO, Phase::ZERO},
      {Phase::NINETY, Phase::NINETY},
      {Phase::ONE_EIGHTY, Phase::ONE_EIGHTY},
      {Phase::TWO_SEVENTY, Phase::TWO_SEVENTY}}},
    {Shift::PI_OVER_TWO,
     {{Phase::ZERO, Phase::NINETY},
      {Phase::NINETY, Phase::ONE_EIGHTY},
      {Phase::ONE_EIGHTY, Phase::TWO_SEVENTY},
      {Phase::TWO_SEVENTY, Phase::ZERO}}},
    {Shift::MINUS_PI_OVER_TWO,
     {{Phase::ZERO, Phase::TWO_SEVENTY},
      {Phase::NINETY, Phase::ZERO},
      {Phase::ONE_EIGHTY, Phase::NINETY},
      {Phase::TWO_SEVENTY, Phase::ONE_EIGHTY}}},
    {Shift::PI,
     {{Phase::ZERO, Phase::ONE_EIGHTY},
      {Phase::NINETY, Phase::TWO_SEVENTY},
      {Phase::ONE_EIGHTY, Phase::ZERO},
      {Phase::TWO_SEVENTY, Phase::NINETY}}}};

Phase getShiftedConvolutionalCodePhase(uint8_t code, Phase current_phase) {
  const Shift shift = ConvolutionalCode.at(code);
  return PhaseShiftMap.at(shift).at(current_phase);
};

} // namespace signal_easel::psk