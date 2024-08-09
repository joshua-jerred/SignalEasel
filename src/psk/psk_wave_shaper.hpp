/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   varicode.cpp
 * @date   2024-08-08
 * @brief  Varicode Implementation
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include <BoosterSeat/math.hpp>

#include <SignalEasel/constants.hpp>
#include <SignalEasel/exception.hpp>
#include <SignalEasel/modulator.hpp>

namespace signal_easel::psk {

enum class Phase { ZERO = 0, NINETY = 1, ONE_EIGHTY = 2, TWO_SEVENTY = 3 };

class PskWaveShaper {
public:
  PskWaveShaper(const uint32_t carrier_frequency, const uint32_t symbol_rate,
                const double amplitude, std::vector<int16_t> &sample_buffer)
      : SAMPLES_PER_SYMBOL_(signal_easel::AUDIO_SAMPLE_RATE / symbol_rate),
        AMPLITUDE_(amplitude), sample_buffer_(sample_buffer) {

    signal_easel::validate(amplitude > 0.0 && amplitude <= 1.0,
                           "Amplitude must be in range (0.0, 1.0]");

    angle_delta_ = bst::TWO_PI * static_cast<double>(carrier_frequency) /
                   static_cast<double>(signal_easel::AUDIO_SAMPLE_RATE);
  }

  void addSymbol(Phase phase, bool filter_end_of_symbol) {
    current_phase_ = phase;
    filter_end_of_symbol_ = filter_end_of_symbol;

    constexpr std::array<double, 4> SHIFT_ANGLE = {0.0, bst::HALF_PI, bst::PI,
                                                   bst::THREE_PI_HALVES};

    for (sample_num_ = 0; sample_num_ < SAMPLES_PER_SYMBOL_; sample_num_++) {
      // Calculate X component of the carrier wave, from -1.0 to 1.0
      double raw_sample = std::sin(carrier_wave_angle_ +
                                   SHIFT_ANGLE[static_cast<uint8_t>(phase)]);

      // Apply the envelope filter to the sample
      envelopeFilter(raw_sample);

      // Convert the wave position to a int16_t sample
      uint16_t sample =
          static_cast<int16_t>((raw_sample * signal_easel::MAX_SAMPLE_VALUE));

      sample_buffer_.push_back(sample);

      carrier_wave_angle_ += angle_delta_;
      if (carrier_wave_angle_ >= bst::TWO_PI) {
        carrier_wave_angle_ -= bst::TWO_PI;
      }
    }

    previous_phase_ = current_phase_;
  }

private:
  /**
   * @brief A filter to remove the sharp edges of the wave during a phase
   * change.
   * @details The filter is a
   * @param[in out] raw_sample - The unfiltered sample
   */
  void envelopeFilter(double &raw_sample) const {
    // Determine if the current sample is in the first half of the symbol
    const bool first_half = sample_num_ < HALF_SAMPLES_PER_SYMBOL_;

    // If the current phase is different from the previous phase, apply the
    // filter to the first half of the symbol. If the next phase is different
    // from the current, apply the filter to the second half of the symbol.
    const bool filter_first_half =
        first_half && current_phase_ != previous_phase_;

    // Apply the filter to the sample
    if ((first_half && filter_first_half) ||
        (!first_half && filter_end_of_symbol_)) {
      raw_sample *=
          std::sin(FILTER_SIN_PI_SCALER_ * static_cast<double>(sample_num_));
    }

    // Apply the amplitude scaling to the sample
    raw_sample *= AMPLITUDE_;
  }

  Phase current_phase_ = Phase::ZERO;
  Phase previous_phase_ = Phase::ZERO;
  bool filter_end_of_symbol_ = false;

  double carrier_wave_angle_ = 0.0;

  double angle_delta_ = 0.0;

  uint32_t sample_num_ = 0;

  const uint32_t SAMPLES_PER_SYMBOL_;
  const uint32_t HALF_SAMPLES_PER_SYMBOL_ = SAMPLES_PER_SYMBOL_ / 2;

  const double AMPLITUDE_;
  const double FILTER_SIN_PI_SCALER_ = bst::PI / SAMPLES_PER_SYMBOL_;

  std::vector<int16_t> &sample_buffer_;
};

} // namespace signal_easel::psk