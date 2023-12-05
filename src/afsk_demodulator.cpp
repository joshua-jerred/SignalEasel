/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   afsk_demodulator.cpp
 * @date   2023-12-05
 * @brief  The AFSK demodulator class implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <cmath>
#include <iostream>

#include <SignalEasel/afsk.hpp>

namespace signal_easel {

AfskDemodulator::ProcessResults AfskDemodulator::processAudioBuffer() {
  AfskDemodulator::ProcessResults results;

  audioBufferToBaseBandSignal(results);
  baseBandToBitStream(results);

  return results;
}

void AfskDemodulator::audioBufferToBaseBandSignal(
    AfskDemodulator::ProcessResults &results) {
  (void)results;

  auto &audio_buffer = getAudioBuffer();

  const int number_of_samples = audio_buffer.size();
  const double sampling_frequency = AUDIO_SAMPLE_RATE;

  std::vector<double> mark_i(number_of_samples);
  std::vector<double> mark_q(number_of_samples);
  std::vector<double> space_i(number_of_samples);
  std::vector<double> space_q(number_of_samples);
  // base_band_signal.resize(number_of_samples);

  for (int i = 0; i < number_of_samples; i++) {
    // normalized sample (between -1 and 1)
    double sample = audio_buffer.at(i) / MAX_SAMPLE_VALUE;

    mark_i.at(i) =
        sample *
        std::sin(TWO_PI_VAL * i * (AFSK_MARK_FREQUENCY / sampling_frequency));
    mark_q.at(i) =
        sample *
        std::cos(TWO_PI_VAL * i * (AFSK_MARK_FREQUENCY / sampling_frequency));
    space_i.at(i) =
        sample *
        std::sin(TWO_PI_VAL * i * (AFSK_SPACE_FREQUENCY / sampling_frequency));
    space_q.at(i) =
        sample *
        std::cos(TWO_PI_VAL * i * (AFSK_SPACE_FREQUENCY / sampling_frequency));

    double mark_i_integ = 0;
    double mark_q_integ = 0;
    double space_i_integ = 0;
    double space_q_integ = 0;

    for (int j = 0; j < (sampling_frequency / AFSK_MARK_FREQUENCY); j++) {
      auto index = i - j;
      if (index > 0) {
        mark_i_integ += mark_i.at(index);
        mark_q_integ += mark_q.at(index);
        space_i_integ += space_i.at(index);
        space_q_integ += space_q.at(index);
      }
    }

    double s1 = mark_i_integ * mark_i_integ + mark_q_integ * mark_q_integ;
    double s2 = space_i_integ * space_i_integ + space_q_integ * space_q_integ;
    double result = s1 - s2;

    base_band_signal_.push_back(result > 0 ? 0xff : 0);
  }
}

void AfskDemodulator::baseBandToBitStream(
    AfskDemodulator::ProcessResults &results) {
  (void)results;
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
  // int32_t mean_samples_between_boundaries = 0;

  constexpr double kClockSkewAlpha = 0.5;
  double clock_skew_accumulator = 0;

  constexpr int32_t kMinSamplesBetweenClockAdjustments = 10;
  int32_t samples_since_last_clock_adjustment = 0;

  for (int8_t sample : base_band_signal_) {
    sample_clock++;
    samples_since_last_clock_adjustment++;

    // trigger a reading of the current symbol's value. True every 40
    // samples.
    if (sample_clock % static_cast<int32_t>(AFSK_SAMPLES_PER_SYMBOL) == 0) {
      /// @todo some form of 'confidence rating' could be helpful here
      // int8_t bit = sample > 0 ? 0xff : 0;
      output_bit_stream_.addBits((unsigned char *)&sample, 1);
      sample_clock = 0;
    }

    samples_since_last_boundary++;

    // detect symbol boundary
    if (sample != previous_sample) {
      num_boundaries++;
      samples_since_last_boundary_sum += samples_since_last_boundary;
      samples_since_last_boundary = 0;
      // mean_samples_between_boundaries =
      //     samples_since_last_boundary_sum / num_boundaries;

      int8_t timing_error_num_samples = (sample_clock % 40) - 20;
      // bool ahead = timing_error_num_samples > 0;
      timing_error_num_samples = abs(timing_error_num_samples);

      clock_skew_count++;
      clock_skew_sum += timing_error_num_samples;
      clock_skew_sum_of_squares +=
          timing_error_num_samples * timing_error_num_samples; // for variance

      // int32_t clock_skew_mean = clock_skew_sum / clock_skew_count;
      // int32_t clock_skew_variance =
      //     (clock_skew_sum_of_squares / clock_skew_count) -
      //     (clock_skew_mean * clock_skew_mean);

      clock_skew_accumulator =
          (kClockSkewAlpha * static_cast<double>(timing_error_num_samples)) +
          (1.0 - kClockSkewAlpha) * clock_skew_accumulator;

      // std::cout << "(" << (ahead ? "+" : "-") <<
      // (int)timing_error_num_samples
      //           << ", " << clock_skew_mean << ", " << clock_skew_variance
      //           << ", " << mean_samples_between_boundaries << ", "
      //           << clock_skew_accumulator << ") ";
    }
    previous_sample = sample;

    // adjust the sample clock in an attempt to synchronize
    if (clock_skew_accumulator > 7 && samples_since_last_clock_adjustment >
                                          kMinSamplesBetweenClockAdjustments) {
      samples_since_last_clock_adjustment = 0;
      if (clock_skew_accumulator > 15) {
        sample_clock += 15;
      }
    }
  }
  output_bit_stream_.pushBufferToBitStream();
}

AfskDemodulator::AsciiResult
AfskDemodulator::lookForString(std::string &output) {
  output.clear();

  const auto &bit_vector = output_bit_stream_.getBitVector();

  // detect syn character
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

  if (char_offset == -1) { // no syn character found/couldn't synchronize
    return AfskDemodulator::AsciiResult::NO_SYN;
  }

  size_t num_bits = output_bit_stream_.getBitStreamLength();
  while (char_offset > 0) { // burn off the bits before the first syn character
    output_bit_stream_.popNextBit();
    char_offset--;
  }

  while (num_bits > 0) {
    uint8_t byte = 0;
    int8_t bit_buffer = 0;

    for (int8_t i = 0; i < 8; i++) {
      bit_buffer = output_bit_stream_.popNextBit();
      if (bit_buffer == -1) {
        std::cout << "Ran out of bits" << std::endl;
        return AfskDemodulator::AsciiResult::NO_EOT;
      }
      byte = byte << 1;
      byte |= bit_buffer;
    }

    if (byte == 0x04) { // End of Transmission (EOT), no more data
      return AfskDemodulator::AsciiResult::SUCCESS;
    } else if (byte != 0x16 && byte != 0x02) { // Ignore SYN and STX
      output += byte;
    }
    num_bits -= 8;
  }

  return AfskDemodulator::AsciiResult::NO_EOT; // no EOT found
}

} // namespace signal_easel