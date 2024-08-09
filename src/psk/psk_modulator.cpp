/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   psk_modulator.cpp
 * @date   2024-06-08
 * @brief  Phase Shift Keying modulator
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <cmath>
#include <iostream>

#include <BoosterSeat/math.hpp>
#include <BoosterSeat/template_tools.hpp>

#include <SignalEasel/constants.hpp>
#include <SignalEasel/psk.hpp>

#include "convolutional_code.hpp"
#include "psk_wave_shaper.hpp"

namespace signal_easel::psk {

void Modulator::encodeString(const std::string &message) {

  uint32_t symbol_rate =
      bst::template_tools::to_underlying(settings_.symbol_rate);

  carrier_wave_angle_ = 0.0f;
  last_symbol_end_filtered_ = 1;

  angle_delta_ = bst::math::TWO_PI *
                 (static_cast<double>(settings_.carrier_frequency) /
                  static_cast<double>(signal_easel::AUDIO_SAMPLE_RATE));
  samples_per_symbol_ = signal_easel::AUDIO_SAMPLE_RATE / symbol_rate;

  addPreamble();
  // Encode each character in the message to the BitStream (varicode)
  static unsigned char zeros[1] = {0x00};
  for (const char &c : message) {
    addVaricode(c);
    bit_stream_.addBits(zeros, 2);
  }
  addPostamble();

  // Push any remaining bits in the buffer to the BitStream
  bit_stream_.pushBufferToBitStream();

  if (settings_.mode == Settings::Mode::BPSK) {
    encodeBpsk();
  } else {
    encodeQpsk();
  }

  bit_stream_.clear();
}

void Modulator::addVaricode(const char c) {
  uint16_t varicode = AsciiToVaricodeArray.at(c);
  unsigned char bits[2];
  int previous_bit = 1;
  for (int i = 0; i < 16; i++) {
    if (!(varicode & (1 << i)) && (!previous_bit)) {
      bits[0] = i > 9 ? varicode >> (i - 9)
                      : varicode << (9 - i); // i > 9 = more than 8 bits
      bits[1] = i > 9 ? varicode << (17 - i) : 0x00;
      bit_stream_.addBits(bits, i - 1);
      break;
    } else {
      previous_bit = varicode & (1 << i);
    }
  }
}

void Modulator::addPreamble() {
  for (size_t i = 0; i < settings_.preamble_length; i++) {
    bit_stream_.addZeroBit();
  }
}

void Modulator::addPostamble() {
  uint32_t postamble_length = settings_.postamble_length;

  postamble_length +=
      32 - (bit_stream_.getBitStreamLength() % 32); // pad to 32 bits

  for (uint32_t i = 0; i < postamble_length; i++) {
    if (settings_.mode == Settings::Mode::QPSK && settings_.fldigi_mode) {
      bit_stream_.addZeroBit();
    } else {
      bit_stream_.addOneBit();
    }
  }
}

void Modulator::encodeBpsk() {
  PskWaveShaper wave_shaper(
      settings_.carrier_frequency,
      bst::template_tools::to_underlying(settings_.symbol_rate),
      settings_.amplitude, audio_buffer_);

  // Swap the phase from 0 to 180 or 180 to 0
  auto swapPhase = [](Phase phase) {
    return phase == Phase::ONE_EIGHTY ? Phase::ZERO : Phase::ONE_EIGHTY;
  };

  Phase last_phase = Phase::ZERO;
  Phase current_phase = Phase::ZERO;

  int bit = bit_stream_.popNextBit();
  int next_bit = bit_stream_.peakNextBit();

  // Request bits from the BitStream until there are no more.
  while (bit != -1) {

    // Encode a 1 by keeping the phase shift the same, encode a 0 by switching
    // phase
    if (bit == 1) {
      current_phase = last_phase;
    } else if (bit == 0) {
      current_phase = swapPhase(last_phase);
    }

    // If the next bit is a 0, the next symbol will shift in phase.
    // This is used to determine if the filter should be applied to the end of
    // this symbol.
    bool next_symbol_shifts = next_bit == 0 ? true : false;
    wave_shaper.addSymbol(current_phase, next_symbol_shifts);

    // Update the state for the next iteration
    last_phase = current_phase;
    bit = bit_stream_.popNextBit();
    next_bit = bit_stream_.peakNextBit();
  }
}

void Modulator::encodeQpsk() {
  PskWaveShaper wave_shaper(
      settings_.carrier_frequency,
      bst::template_tools::to_underlying(settings_.symbol_rate),
      settings_.amplitude, audio_buffer_);

  // Five bit shift register
  uint8_t buffer = 0;

  Phase current_phase = Phase::ZERO;
  Phase next_phase = Phase::ZERO;

  int bit = bit_stream_.popNextBit();
  while (bit != -1) {
    current_phase = getShiftedConvolutionalCodePhase(buffer, current_phase);
    buffer = ((buffer << 1) | bit) & 0x1f;
    next_phase = getShiftedConvolutionalCodePhase(buffer, current_phase);

    bool filter_end = current_phase != next_phase;

    wave_shaper.addSymbol(current_phase, filter_end);

    bit = bit_stream_.popNextBit();
  }
}

} // namespace signal_easel::psk
