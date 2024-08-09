/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   psk.cpp
 * @date   2024-06-08
 * @brief  Phase Shift Keying Implementation
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_PSK_HPP_
#define SIGNAL_EASEL_PSK_HPP_

#include <array>
#include <map>
#include <unordered_map>

#include <SignalEasel/bit_stream.hpp>
#include <SignalEasel/modulator.hpp>

namespace signal_easel::psk {

extern const std::array<uint16_t, 128> AsciiToVaricodeArray;
extern const std::unordered_map<std::string, uint8_t> VaricodeToAsciiMap;

/**
 * @brief Settings for PSK modulation
 */
struct Settings : public signal_easel::Settings {
  /**
   * @brief Enum for the different PSK modes
   */
  enum class Mode { BPSK, QPSK };

  /**
   * @brief Enum for the different symbol rates
   */
  enum class SymbolRate : uint32_t {
    SR_125 = 125U,
    SR_250 = 250U,
    SR_500 = 500U,
    SR_1000 = 1000U
  };

  /**
   * @brief The mode of PSK to use
   */
  Mode mode = Mode::BPSK;

  /**
   * @brief The symbol rate to use
   */
  SymbolRate symbol_rate = SymbolRate::SR_125;

  uint32_t carrier_frequency = 1500;
  uint32_t preamble_length = 64;
  uint32_t postamble_length = 64;

  /**
   * @brief Fldigi, in qpsk mode, uses zeros for the postamble.
   */
  bool fldigi_mode = true;
};

/**
 * @brief PSK Modulator
 */
class Modulator : public signal_easel::Modulator {
public:
  /**
   * @brief Constructor for the PSK Modulator
   * @param settings The settings for the modulator
   */
  Modulator(psk::Settings settings = psk::Settings()) : settings_(settings) {}
  ~Modulator() = default;

  void encodeString(const std::string &input);

private:
  void addVaricode(const char c);
  void addPreamble();
  void addPostamble();
  void addSymbol(double shift, int filter_end);

  void encodeBpsk();
  void encodeQpsk();

  /**
   * @brief The settings for the modulator
   */
  psk::Settings settings_;

  BitStream bit_stream_{};

  double carrier_wave_angle_ = 0.0f;
  int last_symbol_end_filtered_ = 1;
  double angle_delta_ = 0.0f;
  uint32_t samples_per_symbol_ = 0;
};

} // namespace signal_easel::psk

#endif /* SIGNAL_EASEL_PSK_HPP_ */