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
    std::cout << result << " ";
    if (result > 0) {
      result = 1;
    } else {
      result = -1;
    }
    base_band_signal.at(i) = result;
  }
}

void afskBaseBandToBitStream(std::vector<int8_t> &base_band,
                             BitStream &output_bit_stream) {
  int8_t sample_count = 0;
  for (int8_t sample : base_band) {
    sample_count++;
    if (sample_count >= static_cast<int8_t>(mwav::AFSK_SAMPLES_PER_SYMBOL)) {
      int8_t bit = sample > 0 ? 0xff : 0;
      output_bit_stream.addBits((unsigned char *)&bit, 1);
      sample_count = 0;
    }
  }
  output_bit_stream.pushBufferToBitStream();
}

std::string afskBitStreamToAscii(BitStream &bit_stream) {
  std::string output;
  const auto &bit_vector = bit_stream.getBitVector();
  for (int i = 0; i < (int)bit_vector.size(); i++) {
    uint32_t bits = bit_vector.at(i);
    uint8_t byte = (bits >> 24) & 0xFF;
    output += byte;
    byte = (bits >> 16) & 0xFF;
    output += byte;
    byte = (bits >> 8) & 0xFF;
    output += byte;
    byte = (bits) & 0xFF;
    output += byte;
  }
  return output;
}

bool demodulators::afskDecodeAscii(wavgen::Reader &wavgen_reader,
                                   std::string &message) {
  std::vector<int8_t> base_band_signal;
  BitStream bit_stream;

  afskSignalToBaseBand(wavgen_reader, base_band_signal);
  afskBaseBandToBitStream(base_band_signal, bit_stream);
  message = afskBitStreamToAscii(bit_stream);

  return false;
}