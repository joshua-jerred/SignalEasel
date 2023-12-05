/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   afsk_modulator.cpp
 * @date   2023-12-04
 * @brief  The AFSK modulator class implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <cmath>

#include <SignalEasel/afsk.hpp>

namespace signal_easel {

/**
 * @brief Converts a bit to it's bipolar value (0 = 1, 1 = -1)
 * @param bit
 * @return int8_t
 */
inline int8_t convertToBipolar(bool bit) { return bit ? -1 : 1; }

/**
 * @brief For reading a byte MSB first
 * @param byte The input byte
 * @param bit_position The index of the bit to read (0 - 7, MSB - LSB)
 * @return true The bit is set
 * @return false The bit is not set
 */
inline bool isBitSet(uint8_t byte, uint8_t bit_position) {
  return (byte & (1 << (7 - bit_position))) != 0;
}

/**
 * @brief Gets the bipolar value of a bit at a given index from a vector of
 * bytes
 * @param bytes The vector of bytes
 * @param bit_index
 * @return int8_t
 */
inline int8_t getBpBitAtIndex(const std::vector<uint8_t> &bytes,
                              size_t bit_index) {
  const size_t byte_index = bit_index / 8;
  const size_t bit_offset = bit_index % 8;
  return convertToBipolar(isBitSet(bytes.at(byte_index), bit_offset));
}

bool AfskModulator::encodeBytes(const std::vector<uint8_t> &input_bytes) {
  if (input_bytes.empty()) {
    return false;
  }

  auto bytes = input_bytes;

  if (settings_.bit_encoding == AfskSettings::BitEncoding::NRZI) {
    convertToNRZI(bytes);
  }

  const int kNumBitsToEncode = input_bytes.size() * 8;

  /// @brief "index" is the used to determine when to go to the next bit. It is
  /// not actually used as an index.
  int index = 0;
  /// @brief "prev_index" is the previous value of "index"
  int prev_index = 0;

  /// @brief The index of the current bit in the vector of bytes
  size_t bit_index = 0;

  // get the first bit bp bit
  current_bipolar_bit_ = getBpBitAtIndex(bytes, bit_index);
  bit_index++;

  uint32_t iterations = kNumBitsToEncode * SAMPLES_PER_SYMBOL_;
  for (uint32_t i = 2; i < iterations; i++) {
    index = std::floor((float)i / (float)SAMPLES_PER_SYMBOL_);
    prev_index = std::floor((float)(i - 1) / (float)SAMPLES_PER_SYMBOL_);

    if (index != prev_index) { // Go to the next bit
      previous_bipolar_bit_ = current_bipolar_bit_;
      current_bipolar_bit_ = getBpBitAtIndex(bytes, bit_index);
      bit_index++;
    } else {
      previous_bipolar_bit_ = current_bipolar_bit_;
    }

    // Integrate the bipolar bit
    integral_value_ += ((current_bipolar_bit_ + previous_bipolar_bit_) / 2);

    // Only write the OVER_SAMPLE_FACTOR_'th sample
    if (i % OVER_SAMPLE_FACTOR_ == 0) {
      constexpr double LHS_MULTIPLIER = TWO_PI_VAL * CENTER_OVER_SAMPLE_FREQ_;
      constexpr double RHS_MULTIPLIER = TWO_PI_VAL * DELTA_OVER_SAMPLE_FREQ_;
      double lhs = LHS_MULTIPLIER * static_cast<double>(i);
      double rhs = RHS_MULTIPLIER * static_cast<double>(integral_value_);
      int16_t sample =
          std::cos(lhs + rhs) * settings_.amplitude * MAX_SAMPLE_VALUE;
      addAudioSample(sample);
    }
  }

  return true;
}

void AfskModulator::convertToNRZI(std::vector<uint8_t> &data) {
  for (size_t i = 0; i < data.size(); i++) {
    for (size_t j = 0; j < 8; j++) {
      bool bit = isBitSet(data.at(i), j);
      if (!bit && nrzi_previous_tone_mark_) {
        nrzi_previous_tone_mark_ = false;
        data.at(i) ^= (1 << (7 - j));
      } else if (bit && !nrzi_previous_tone_mark_) {
        nrzi_previous_tone_mark_ = true;
        data.at(i) ^= (1 << (7 - j));
      }
    }
  }
}

} // namespace signal_easel