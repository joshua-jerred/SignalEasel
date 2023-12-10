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

#include <SignalEasel/constants.hpp>

#include <SignalEasel/bit_stream.hpp>
#include <SignalEasel/demodulator.hpp>
#include <SignalEasel/modulator.hpp>
#include <SignalEasel/receiver.hpp>

namespace signal_easel {
namespace aprs {
class Receiver;
} // namespace aprs

/**
 * @brief Audio Frequency Shift Keying (AFSK)
 */
namespace afsk {

inline constexpr uint32_t AFSK_BAUD_RATE = 1200;
inline constexpr uint32_t AFSK_MARK_FREQUENCY = 1200;
inline constexpr uint32_t AFSK_CENTER_FREQUENCY = 1700;
inline constexpr uint32_t AFSK_SPACE_FREQUENCY = 2200;
inline constexpr uint32_t AFSK_FREQUENCY_DEVIATION =
    (AFSK_SPACE_FREQUENCY - AFSK_MARK_FREQUENCY) / 2;
inline constexpr uint32_t AFSK_SAMPLES_PER_SYMBOL =
    AUDIO_SAMPLE_RATE / AFSK_BAUD_RATE;

inline constexpr uint16_t AFSK_ASCII_PREAMBLE_LENGTH = 30;

inline constexpr uint16_t AFSK_BP_FILTER_ORDER = 4;

inline constexpr double AFSK_BP_MARK_LOWER_CUTOFF = 1000;
inline constexpr double AFSK_BP_MARK_UPPER_CUTOFF = 1400;
inline constexpr double AFSK_BP_SPACE_LOWER_CUTOFF = 2000;
inline constexpr double AFSK_BP_SPACE_UPPER_CUTOFF = 2400;

inline constexpr double AFSK_BP_INCLUDED_BANDWIDTH =
    AFSK_BP_MARK_UPPER_CUTOFF - AFSK_BP_MARK_LOWER_CUTOFF +
    AFSK_BP_SPACE_UPPER_CUTOFF - AFSK_BP_SPACE_LOWER_CUTOFF;

inline constexpr double AFSK_MINIMUM_SNR = -50.0;
inline constexpr double AFSK_SNR_THRESHOLD = -10.0;

// seconds * samples per second
inline constexpr size_t AFSK_RECEIVER_SAMPLE_BUFFER_SIZE =
    5 * AUDIO_SAMPLE_RATE;
// Minimum number of samples that need to be received before demodulation
inline constexpr size_t AFSK_RECEIVED_MIN_SAMPLES = 2000;

/**
 * @brief Settings for AFSK modulation/demodulation.
 */
struct Settings : public signal_easel::Settings {
  enum class BitEncoding { STANDARD, NRZI };

  Settings::BitEncoding bit_encoding = BitEncoding::STANDARD;

  /**
   * @brief If true, a string will be surrounded by SYN and EOT characters.
   */
  bool include_ascii_padding = true;
};

/**
 * @brief Modulates data into an AFSK signal.
 */
class Modulator : public DataModulator {
public:
  Modulator(afsk::Settings settings = afsk::Settings())
      : settings_(std::move(settings)) {}
  ~Modulator() = default;

protected:
  void encodeBytes(const std::vector<uint8_t> &input_bytes) override;

private:
  /**
   * @brief Converts the data to NRZI for APRS mode.
   * @details NRZI encoding (Non Return to Zero Inverted), 0 is encoded as a
   * *change in tone*, while 1 is encoded as *no change*.
   * @param data The data to convert
   */
  void convertToNRZI(std::vector<uint8_t> &data);

  afsk::Settings settings_;

  bool nrzi_previous_tone_mark_ = false;
  int8_t current_bipolar_bit_ = 0;
  int8_t previous_bipolar_bit_ = 0;
  int32_t integral_value_ = 0;

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

/**
 * @brief Demodulates an AFSK signal into data.
 */
class Demodulator : public signal_easel::Demodulator {
public:
  friend class Receiver;
  friend class aprs::Receiver;

  /**
   * @brief Stats related to the processing of the audio buffer.
   * @details Used to determine if the signal is valid.
   */
  struct ProcessResults {
    /**
     * @brief RMS of the input signal (received audio power).
     */
    double rms = 0.0;

    /**
     * @brief Estimated Signal to Noise Ratio (SNR) of the input signal.
     */
    double snr = 0.0;
  };

  Demodulator(afsk::Settings settings = afsk::Settings())
      : signal_easel::Demodulator(settings),
        afsk_settings_(std::move(settings)) {}
  ~Demodulator() = default;

  ProcessResults processAudioBuffer();

  enum class AsciiResult { SUCCESS, NO_SYN, NO_EOT };

  AsciiResult lookForString(std::string &output);

#ifdef PULSE_AUDIO_ENABLED
  // bool detectSignal() override;
#endif

protected:
  BitStream output_bit_stream_{};

private:
  /**
   * @brief Takes the raw signal and converts it into the FSK baseband signal.
   * @details
   * @todo This needs to be cleaned up.
   * @param results (out) results The results of the processing.
   */
  void audioBufferToBaseBandSignal(ProcessResults &results);

  /**
   * @brief Takes the base band signal and converts it into a bit stream if the
   * signal is valid.
   * @param results (out) results The results of the processing.
   */
  void baseBandToBitStream(ProcessResults &results);

  std::vector<uint8_t> base_band_signal_{};

  afsk::Settings afsk_settings_;
};

/**
 * @brief For receiving AFSK signals via a pulse audio stream.
 */
class Receiver : public signal_easel::Receiver {
public:
  Receiver(afsk::Settings settings = afsk::Settings())
      : signal_easel::Receiver(settings), demodulator_(settings),
        afsk_settings_(settings) {}
  ~Receiver() = default;

  void process() override;

protected:
  bool detectSignal(const PulseAudioBuffer &audio_buffer);
  virtual void decode();

  afsk::Demodulator demodulator_;

  afsk::Settings afsk_settings_;

  std::vector<int16_t> receive_buffer_{};
};

} // namespace afsk
} // namespace signal_easel

#endif /* MWAV_AFSK_HPP_ */