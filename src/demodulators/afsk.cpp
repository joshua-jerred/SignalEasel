/**
 * @file psk.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief AFSK Demodulator
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "bit-stream.h"
#include "demodulators.hpp"
#include "mwav_constants.hpp"

/**
 * @brief Takes the raw signal and converts it into the FSK baseband signal.
 *
 * @details Big thanks to https://www.notblackmagic.com/bitsnpieces/afsk/
 * once again!
 *
 * @param wavgen_reader
 * @param bit_stream
 * @return true
 * @return false
 */
void afskSignalToBaseBand(wavgen::Reader &wavgen_reader,
                          std::vector<int8_t> &base_band_signal) {
  constexpr double markFrequency = 1200;
  constexpr double spaceFrequency = 2200;
  std::vector<int16_t> wave_data;
  wavgen_reader.getAllSamples(wave_data);
  const int number_of_samples = wave_data.size();
  const double sampling_frequency = wavgen_reader.getSampleRate();

  std::vector<double> mark_i(number_of_samples);
  std::vector<double> mark_q(number_of_samples);
  std::vector<double> space_i(number_of_samples);
  std::vector<double> space_q(number_of_samples);
  base_band_signal.resize(number_of_samples);

  for (int i = 0; i < number_of_samples; i++) {
    double sample = wave_data.at(i) / 32768.0;

    mark_i.at(i) =
        sample * sin(2 * M_PI * i * (markFrequency / sampling_frequency));
    mark_q.at(i) =
        sample * cos(2 * M_PI * i * (markFrequency / sampling_frequency));
    space_i.at(i) =
        sample * sin(2 * M_PI * i * (spaceFrequency / sampling_frequency));
    space_q.at(i) =
        sample * cos(2 * M_PI * i * (spaceFrequency / sampling_frequency));

    double mark_i_integ = 0;
    double mark_q_integ = 0;
    double space_i_integ = 0;
    double space_q_integ = 0;

    for (int j = 0; j < (sampling_frequency / markFrequency); j++) {
      auto index = i - j;
      if (index > 0) {
        mark_i_integ += mark_i.at(index);
        mark_q_integ += mark_q.at(index);
        space_i_integ += space_i.at(index);
        space_q_integ += space_q.at(index);
      } else {
        mark_i_integ += 0;
        mark_q_integ += 0;
        space_i_integ += 0;
        space_q_integ += 0;
      }
    }

    double s1 = mark_i_integ * mark_i_integ + mark_q_integ * mark_q_integ;
    double s2 = space_i_integ * space_i_integ + space_q_integ * space_q_integ;
    double result = s1 - s2;
    // std::cout << result << " ";
    // if (result > 0) {
    //   result = 1;
    // } else {
    //   result = -1;
    // }
    base_band_signal.at(i) = result > 0 ? 0xff : 0;
  }
}

void afskBaseBandToBitStream(std::vector<int8_t> &base_band,
                             BitStream &output_bit_stream) {
  /// @brief The sample clock counts up to 40 and then resets.
  /// @details Symbols are 40 samples long. This clock is used to determine when
  /// to add a bit to the bit stream.
  int32_t sample_clock = 0;
  /// @brief Used to detect the actual symbol boundary.
  int8_t previous_sample = 0;

  int32_t clock_skew_sum = 0;
  int32_t clock_skew_sum_of_squares = 0;
  int32_t clock_skew_count = 0;

  int32_t samples_since_last_boundary = 0;
  int32_t samples_since_last_boundary_sum = 0;
  int32_t num_boundaries = 0;
  int32_t mean_samples_between_boundaries = 0;

  constexpr double kClockSkewAlpha = 0.5;
  double clock_skew_accumulator = 0;

  constexpr int32_t kMinSamplesBetweenClockAdjustments = 10;
  int32_t samples_since_last_clock_adjustment = 0;

  for (int8_t sample : base_band) {
    sample_clock++;
    samples_since_last_clock_adjustment++;

    // trigger a reading of the current symbol's value. True every 40
    // samples.
    if (sample_clock % static_cast<int32_t>(mwav::AFSK_SAMPLES_PER_SYMBOL) ==
        0) {
      /// @todo some form of 'confidence rating' could be helpful here
      // int8_t bit = sample > 0 ? 0xff : 0;
      output_bit_stream.addBits((unsigned char *)&sample, 1);
      sample_clock = 0;
    }

    samples_since_last_boundary++;

    // detect symbol boundary
    if (sample != previous_sample) {
      num_boundaries++;
      samples_since_last_boundary_sum += samples_since_last_boundary;
      samples_since_last_boundary = 0;
      mean_samples_between_boundaries =
          samples_since_last_boundary_sum / num_boundaries;

      int8_t timing_error_num_samples = (sample_clock % 40) - 20;
      bool ahead = timing_error_num_samples > 0;
      timing_error_num_samples = abs(timing_error_num_samples);

      clock_skew_count++;
      clock_skew_sum += timing_error_num_samples;
      clock_skew_sum_of_squares +=
          timing_error_num_samples * timing_error_num_samples; // for variance

      int32_t clock_skew_mean = clock_skew_sum / clock_skew_count;
      int32_t clock_skew_variance =
          (clock_skew_sum_of_squares / clock_skew_count) -
          (clock_skew_mean * clock_skew_mean);

      clock_skew_accumulator =
          (kClockSkewAlpha * static_cast<double>(timing_error_num_samples)) +
          (1.0 - kClockSkewAlpha) * clock_skew_accumulator;

      std::cout << "(" << (ahead ? "+" : "-") << (int)timing_error_num_samples
                << ", " << clock_skew_mean << ", " << clock_skew_variance
                << ", " << mean_samples_between_boundaries << ", "
                << clock_skew_accumulator << ") ";
    }
    previous_sample = sample;

    if (clock_skew_accumulator > 7 && samples_since_last_clock_adjustment >
                                          kMinSamplesBetweenClockAdjustments) {
      samples_since_last_clock_adjustment = 0;
      if (clock_skew_accumulator > 15) {
        sample_clock += 15;
      }
    }
  }
  output_bit_stream.pushBufferToBitStream();
}

bool afskBitStreamToAscii(BitStream &bit_stream, std::string &output) {
  output.clear();
  const auto &bit_vector = bit_stream.getBitVector();

  // detect syn characters and sync to them
  int8_t char_offset = -1;
  for (uint32_t word : bit_vector) {
    for (int8_t i = 0; i < 32; i++) {
      uint8_t byte = (word >> (24 - i)) & 0xFF;
      if (byte == 0x16) {
        char_offset = i;
        break;
      }
    }
  }

  if (char_offset == -1) {
    std::cout << "Could not find a syn character" << std::endl;
    return false;
  }

  std::cout << std::endl << "char_offset: " << (int)char_offset << std::endl;

  size_t num_bits = bit_stream.getBitStreamLength();
  while (char_offset > 0) {
    bit_stream.popNextBit();
    char_offset--;
  }

  while (num_bits > 0) {
    uint8_t byte = 0;
    int8_t bit_buffer = 0;

    for (int8_t i = 0; i < 8; i++) {
      bit_buffer = bit_stream.popNextBit();
      if (bit_buffer == -1) {
        std::cout << "Ran out of bits" << std::endl;
        return false;
      }
      byte = byte << 1;
      byte |= bit_buffer;
    }

    output += byte;
    num_bits -= 8;
  }

  return true;
}

bool demodulators::afskDecodeAscii(wavgen::Reader &wavgen_reader,
                                   std::string &message) {
  std::vector<int8_t> base_band_signal;
  BitStream bit_stream;
  bool success = false;

  afskSignalToBaseBand(wavgen_reader, base_band_signal);
  afskBaseBandToBitStream(base_band_signal, bit_stream);
  success = afskBitStreamToAscii(bit_stream, message);
  std::cout << std::endl << message << std::endl;

  return success;
}