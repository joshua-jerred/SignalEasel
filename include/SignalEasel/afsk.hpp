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
inline constexpr uint32_t AFSK_SPACE_FREQUENCY = 2200;
inline constexpr uint32_t AFSK_SAMPLES_PER_SYMBOL =
    AUDIO_SAMPLE_RATE / AFSK_BAUD_RATE;

struct AfskSettings : public GlobalSettings {
  enum class BitEncoding { STANDARD, NRZI };

  AfskSettings::BitEncoding bit_encoding = BitEncoding::STANDARD;
};

class AfskModulator : public Modulator {
public:
  AfskModulator(AfskSettings settings = AfskSettings())
      : settings_(std::move(settings)) {}
  ~AfskModulator() = default;

  /**
   * @brief Adds bytes of data to the output buffer
   *
   * @param data
   * @return true
   * @return false
   */
  bool addBytes(const std::vector<uint8_t> &data);

  /**
   * @brief Adds a string of data to the output buffer
   *
   * @param data
   * @return true
   * @return false
   */
  bool addString(const std::string &data);

private:
  AfskSettings settings_;
};

} // namespace signal_easel

#endif /* MWAV_AFSK_HPP_ */