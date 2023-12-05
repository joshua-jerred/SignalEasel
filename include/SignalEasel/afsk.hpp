/**
 * @file afsk.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief AFSK related modulation/demodulation
 * @date 2023-12-03
 * @copyright Copyright (c) 2023
 */

#ifndef SIGNAL_EASEL_AFSK_HPP_
#define SIGNAL_EASEL_AFSK_HPP_

#include <vector>

#include "signal_easel.hpp"

namespace signal_easel {

inline constexpr uint32_t AFSK_BAUD_RATE = 1200;
inline constexpr uint32_t AFSK_MARK_FREQUENCY = 1200;
inline constexpr uint32_t AFSK_CENTER_FREQUENCY = 1700;
inline constexpr uint32_t AFSK_SPACE_FREQUENCY = 2200;
inline constexpr uint32_t AFSK_FREQUENCY_DEVIATION =
    (AFSK_SPACE_FREQUENCY - AFSK_MARK_FREQUENCY) / 2;
inline constexpr uint32_t AFSK_SAMPLES_PER_SYMBOL =
    AUDIO_SAMPLE_RATE / AFSK_BAUD_RATE;
struct AfskSettings : public GlobalSettings {
  enum class BitEncoding { STANDARD, NRZI };

  AfskSettings::BitEncoding bit_encoding = BitEncoding::STANDARD;
};

class AfskModulator : public DataModulator {
public:
  AfskModulator(AfskSettings settings = AfskSettings())
      : settings_(std::move(settings)) {}
  ~AfskModulator() = default;

private:
  void encodeBytes(const std::vector<uint8_t> &data) override;

  /**
   * @brief Converts the data to NRZI for APRS mode.
   * @details NRZI encoding (Non Return to Zero Inverted), 0 is encoded as a
   * *change in tone*, while 1 is encoded as *no change*.
   * @param data The data to convert
   */
  void convertToNRZI(std::vector<uint8_t> &data);

  AfskSettings settings_;

  bool nrzi_previous_tone_mark_ = false;
  int8_t current_bipolar_bit_ = 0;
  int8_t previous_bipolar_bit_ = 0;
  int8_t integral_value_ = 0;

  static constexpr uint32_t OVER_SAMPLE_FACTOR_ = 4;
  static constexpr uint32_t SAMPLE_FREQUENCY_ =
      AUDIO_SAMPLE_RATE * OVER_SAMPLE_FACTOR_;
  static constexpr uint32_t SAMPLES_PER_SYMBOL_ =
      SAMPLE_FREQUENCY_ / AFSK_BAUD_RATE;

  static constexpr double CENTER_OVER_SAMPLE_FREQ_ =
      static_cast<double>(AFSK_CENTER_FREQUENCY) / SAMPLE_FREQUENCY_;
  static constexpr double DELTA_OVER_SAMPLE_FREQ_ =
      static_cast<double>(AFSK_FREQUENCY_DEVIATION) / SAMPLE_FREQUENCY_;
};

} // namespace signal_easel

#endif /* MWAV_AFSK_HPP_ */