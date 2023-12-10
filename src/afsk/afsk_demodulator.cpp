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

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include <SignalEasel/afsk.hpp>
#include <SignalEasel/exception.hpp>

#include "band_pass_filter.hpp"

namespace signal_easel {

AfskDemodulator::ProcessResults AfskDemodulator::processAudioBuffer() {
  AfskDemodulator::ProcessResults results;

  audioBufferToBaseBandSignal(results);
  baseBandToBitStream(results);

  return results;
}

void AfskDemodulator::audioBufferToBaseBandSignal(
    AfskDemodulator::ProcessResults &results) {
  base_band_signal_.clear();
  const auto &samples_buffer = getAudioBuffer();
  std::vector<double> unfiltered(samples_buffer.begin(), samples_buffer.end());

  auto filtered_audio =
      bandPassFilter(unfiltered, AUDIO_SAMPLE_RATE_D, AFSK_BP_MARK_LOWER_CUTOFF,
                     AFSK_BP_SPACE_UPPER_CUTOFF, AFSK_BP_FILTER_ORDER);

  // calculate RMS for SNR
  auto lower_audio =
      bandPassFilter(unfiltered, AUDIO_SAMPLE_RATE_D, AFSK_BP_MARK_LOWER_CUTOFF,
                     AFSK_BP_MARK_UPPER_CUTOFF, AFSK_BP_FILTER_ORDER);
  auto upper_audio = bandPassFilter(
      unfiltered, AUDIO_SAMPLE_RATE_D, AFSK_BP_SPACE_LOWER_CUTOFF,
      AFSK_BP_SPACE_UPPER_CUTOFF, AFSK_BP_FILTER_ORDER);
  if (lower_audio.size() != upper_audio.size()) {
    throw Exception(Exception::Id::NON_MATCHING_FILTER_VECTORS);
  }

  double rms = 0;
  for (size_t i = 0; i < lower_audio.size(); i++) {
    double combined =
        (lower_audio.at(i) + upper_audio.at(i)) / AFSK_BP_INCLUDED_BANDWIDTH;
    rms += combined * combined;
  }
  rms = std::sqrt(rms / lower_audio.size());

  // calculate the RMS for a wider signal
  constexpr double k_wide_band_lower_cutoff = 500;
  constexpr double k_wide_band_upper_cutoff = 2700;
  constexpr double k_wide_band_included_bandwidth =
      k_wide_band_upper_cutoff - k_wide_band_lower_cutoff;
  constexpr double k_wide_band_rms_additional_weight = 0.5;
  auto wide_audio =
      bandPassFilter(unfiltered, AUDIO_SAMPLE_RATE_D, k_wide_band_lower_cutoff,
                     k_wide_band_upper_cutoff, AFSK_BP_FILTER_ORDER);
  double wide_rms = 0;
  for (double &sample : wide_audio) {
    double adjusted = (sample / k_wide_band_included_bandwidth) +
                      k_wide_band_rms_additional_weight;
    wide_rms += adjusted * adjusted;
  }
  wide_rms = std::sqrt(wide_rms / wide_audio.size());

  // calculate SNR
  results.rms = rms;
  results.snr = 20 * std::log10(rms / wide_rms);

  if (results.snr < AFSK_MINIMUM_SNR) {
    results.snr = AFSK_MINIMUM_SNR;
  }

  const size_t k_number_of_samples = filtered_audio.size();

  std::vector<double> mark_i(k_number_of_samples);
  std::vector<double> mark_q(k_number_of_samples);
  std::vector<double> space_i(k_number_of_samples);
  std::vector<double> space_q(k_number_of_samples);

  for (int i = 0; i < static_cast<int>(k_number_of_samples); i++) {
    // normalized sample (between -1 and 1)
    double sample =
        filtered_audio.at(i) / static_cast<double>(MAX_SAMPLE_VALUE);

    mark_i.at(i) =
        sample *
        std::sin(TWO_PI_VAL * i * (AFSK_MARK_FREQUENCY / AUDIO_SAMPLE_RATE_D));
    mark_q.at(i) =
        sample *
        std::cos(TWO_PI_VAL * i * (AFSK_MARK_FREQUENCY / AUDIO_SAMPLE_RATE_D));
    space_i.at(i) =
        sample *
        std::sin(TWO_PI_VAL * i * (AFSK_SPACE_FREQUENCY / AUDIO_SAMPLE_RATE_D));
    space_q.at(i) =
        sample *
        std::cos(TWO_PI_VAL * i * (AFSK_SPACE_FREQUENCY / AUDIO_SAMPLE_RATE_D));

    double mark_i_integ = 0;
    double mark_q_integ = 0;
    double space_i_integ = 0;
    double space_q_integ = 0;

    for (int j = 0;
         j < static_cast<int>(AUDIO_SAMPLE_RATE / AFSK_MARK_FREQUENCY); j++) {
      auto index = i - j;
      if (index > 0) {
        mark_i_integ += mark_i.at(index);
        mark_q_integ += mark_q.at(index);
        space_i_integ += space_i.at(index);
        space_q_integ += space_q.at(index);
      }
    }

    double correlator_mark =
        mark_i_integ * mark_i_integ + mark_q_integ * mark_q_integ;
    double correlator_space =
        space_i_integ * space_i_integ + space_q_integ * space_q_integ;
    double result = correlator_mark - correlator_space;

    base_band_signal_.push_back(result > 0.0 ? 0xff : 0x00);
  }
}

void AfskDemodulator::baseBandToBitStream(
    AfskDemodulator::ProcessResults &results) {
  output_bit_stream_ = BitStream();
  /// @brief The sample clock counts up to 40 and then resets.
  /// @details Symbols are 40 samples long. This clock is used to determine when
  /// to add a bit to the bit stream.
  int32_t sample_clock = 0;
  /// @brief Used to detect the actual symbol boundary.
  uint8_t previous_sample = 0;

  int32_t clock_skew_sum = 0;
  int32_t clock_skew_sum_of_squares = 0;
  int32_t clock_skew_count = 0;

  int32_t clock_skew_mean = 0;
  int32_t clock_skew_variance = 0;

  int32_t samples_since_last_boundary = 0;
  int32_t samples_since_last_boundary_sum = 0;
  int32_t num_boundaries = 0;
  int32_t mean_samples_between_boundaries = 0;

  uint32_t num_clock_syncs = 0;

  // bool ahead = false;

  constexpr double k_clock_skew_alpha = 0.5;
  double clock_skew_accumulator = 0;

  constexpr int32_t k_min_samples_between_clock_adjustments = 10;
  int32_t samples_since_last_clock_adjustment = 0;

  for (uint8_t sample : base_band_signal_) {
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
      mean_samples_between_boundaries =
          samples_since_last_boundary_sum / num_boundaries;

      int timing_error_num_samples = sample_clock % 40 - 20;
      // ahead = timing_error_num_samples > 0;
      timing_error_num_samples = std::abs(timing_error_num_samples);

      clock_skew_count++;
      clock_skew_sum += timing_error_num_samples;
      clock_skew_sum_of_squares +=
          timing_error_num_samples * timing_error_num_samples; // for variance

      clock_skew_mean = clock_skew_sum / clock_skew_count;
      clock_skew_variance = (clock_skew_sum_of_squares / clock_skew_count) -
                            (clock_skew_mean * clock_skew_mean);

      clock_skew_accumulator =
          (k_clock_skew_alpha * static_cast<double>(timing_error_num_samples)) +
          (1.0 - k_clock_skew_alpha) * clock_skew_accumulator;

      // std::cout << "(" << (ahead ? "+" : "-") <<
      // (int)timing_error_num_samples
      //           << ", " << clock_skew_mean << ", " << clock_skew_variance
      //           << ", " << mean_samples_between_boundaries << ", "
      //           << clock_skew_accumulator << ") ";
    }
    previous_sample = sample;

    // adjust the sample clock in an attempt to synchronize
    if (clock_skew_accumulator > 7 &&
        samples_since_last_clock_adjustment >
            k_min_samples_between_clock_adjustments) {
      samples_since_last_clock_adjustment = 0;
      if (clock_skew_accumulator > 15) {
        sample_clock += 15;
        num_clock_syncs++;
      } else {
        // if (ahead) {
        // sample_clock += 5;
        // } else {
        // sample_clock -= 5;
        // }
      }
    }
  }
  // std::cout << std::endl
  // << "MSBB: " << mean_samples_between_boundaries << std::endl;
  // std::cout << "CSA: " << clock_skew_accumulator << std::endl;
  // std::cout << "CSM: " << clock_skew_mean << std::endl;
  // std::cout << "CSV: " << clock_skew_variance << std::endl;
  // std::cout << "NCS: " << num_clock_syncs << std::endl;
  output_bit_stream_.pushBufferToBitStream();
}

AfskDemodulator::AsciiResult
AfskDemodulator::lookForString(std::string &output) {
  output.clear();

  const auto &bit_vector = output_bit_stream_.getBitVector();

  constexpr uint8_t k_syn_character = 0x16;

  // detect syn character
  int8_t char_offset = -1;
  for (uint32_t word : bit_vector) {
    for (int8_t i = 0; i < 32; i++) {
      uint8_t byte = (word >> (24 - i)) & 0xFF;
      if (byte == k_syn_character) {
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
        // std::cout << "Ran out of bits" << std::endl;
        return AfskDemodulator::AsciiResult::NO_EOT;
      }
      byte = byte << 1;
      byte |= bit_buffer;
    }

    if (byte == 0x04) { // End of Transmission (EOT), no more data
      return AfskDemodulator::AsciiResult::SUCCESS;
    }

    // Ignore SYN and STX
    if (byte != 0x16 && byte != 0x02) {
      output += static_cast<unsigned char>(byte);
    }
    num_bits -= 8;
  }

  return AfskDemodulator::AsciiResult::NO_EOT; // no EOT found
}

} // namespace signal_easel