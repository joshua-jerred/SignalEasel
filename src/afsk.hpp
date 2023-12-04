/**
 * @file afsk.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief AFSK related modulation/demodulation
 * @date 2023-12-03
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_AFSK_HPP_
#define MWAV_AFSK_HPP_

#include <vector>

#include "mwav.hpp"

namespace mwav {

inline constexpr uint32_t AFSK_BAUD_RATE = 1200;
inline constexpr uint32_t AFSK_MARK_FREQUENCY = 1200;
inline constexpr uint32_t AFSK_SPACE_FREQUENCY = 2200;
inline constexpr uint32_t AFSK_SAMPLES_PER_SYMBOL =
    AUDIO_SAMPLE_RATE / AFSK_BAUD_RATE;

struct AfskSettings : public MwavSettings {
  enum class BitEncoding { STANDARD, NRZI };

  AfskSettings::BitEncoding bit_encoding = BitEncoding::STANDARD;
};

class AfskModulator : public Modulator {
public:
  AfskModulator(AfskSettings settings = AfskSettings()) : settings_(settings) {}
  ~AfskModulator() = default;

  bool addBinaryData(const std::vector<uint8_t> &data);
  bool addStringData(const std::string &data);

private:
  const AfskSettings settings_;
};

} // namespace mwav

#endif /* MWAV_AFSK_HPP_ */