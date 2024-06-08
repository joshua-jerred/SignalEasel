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
  // static unsigned char zeros[1] = {0x00};
  for (size_t i = 0; i < settings_.preamble_length; i++) {
    bit_stream_.addZeroBit();
  }
}

void Modulator::addPostamble() {
  // static unsigned char zeros[1] = {0x00};
  // static unsigned char ones[1] = {0xFF};

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

void Modulator::addSymbol(double shift, int filter_end) {
  const double power = 2.0;
  const double roll_off = 2.9;
  const double amplitude = .5;

  double time = 0 - (samples_per_symbol_ / 2);
  for (uint32_t i = 0; i < samples_per_symbol_; i++) {
    double unfiltered = std::cos(carrier_wave_angle_ + shift);
    double filter = std::pow(
        std::cos((std::abs(time) / samples_per_symbol_) * roll_off), power);
    if (!last_symbol_end_filtered_ && (time < 0)) {
      filter = 1;
    }
    if (!filter_end && (time > 0)) { // Remove filter from end of symbol
      filter = 1;
    }
    double sample = amplitude * filter * unfiltered * 32768;
    std::cout << sample << std::endl;
    addAudioSample(sample); // write sample to wav file

    carrier_wave_angle_ += angle_delta_;
    time += 1;
    if (carrier_wave_angle_ > 2 * M_PI) {
      carrier_wave_angle_ -= 2 * M_PI;
    }
  }
  last_symbol_end_filtered_ = filter_end;
}

void Modulator::encodeBpsk() {
  int bit = bit_stream_.popNextBit();
  int next_bit = bit_stream_.peakNextBit();
  int last_phase = 0; // 0 = 0, 1 = M_PI
  while (bit != -1) {
    int filter_end;
    if (bit) { // Encode a 1 by keeping the phase shift the same
      filter_end = next_bit == 1
                       ? 0
                       : 1; // If next bit is 1, do not filter end of symbol.
      addSymbol(last_phase ? 0 : M_PI, filter_end);
    } else if (!bit) { // Encode a 0 by switching phase
      filter_end = next_bit == 0
                       ? 0
                       : 1; // If next bit is 0, do not filter end of symbol.
      addSymbol(last_phase ? M_PI : 0, filter_end);
      last_phase = !last_phase;
    }
    // last_phase ? "1" : "0";
    bit = bit_stream_.popNextBit();
    next_bit = bit_stream_.peakNextBit();
  }
}

void Modulator::encodeQpsk() {
  unsigned char buffer = 0;
  int bit = bit_stream_.popNextBit();
  int shift = 0;
  while (bit != -1) {
    buffer = ((buffer << 1) | bit) & 0x1f;
    int filter_end = 0;
    if ((int)shift != (int)ConvolutionalCode.at(buffer)) {
      filter_end = 1;
    }
    shift += ConvolutionalCode.at(buffer);
    addSymbol(shift, filter_end);
    bit = bit_stream_.popNextBit();
  }
}

} // namespace signal_easel::psk
