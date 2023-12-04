/**
 * @file mwav.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Common header for the MWAV library
 * @date 2023-12-03
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_HPP_
#define MWAV_HPP_

#include <cstdint>
#include <exception>
#include <string>

/**
 * @brief The namespace used for the MWAV library
 */
namespace mwav {

class MWAVException : public std::exception {
public:
  MWAVException(const std::string &message) : message(message) {}
  virtual const char *what() const throw() { return message.c_str(); }

private:
  std::string message;
};

/**
 * @brief The sample rate of the audio
 * @details This value was chosen as it's cleanly devisable by 1200 and 60,
 * this is helpful for AFSK and SSTV. Choosing the standard 44100 creates
 * problems, especially with SSTV which was created in the days of 60Hz.
 */
inline constexpr uint32_t AUDIO_SAMPLE_RATE = 48000;

/**
 * @brief The number of bits per sample (16-bit PCM)
 */
inline constexpr uint32_t SAMPLE_RESOLUTION = 16;

/**
 * @brief The maximum value of a sample (2^15 - 1)
 */
inline constexpr uint32_t MAX_SAMPLE_VALUE = 32767;

/**
 * @brief Constant for PI used throughout the project
 */
inline constexpr double PI = 3.14159265358979323846;

/**
 * @brief Constant for 2*PI used throughout the project
 */
inline constexpr double TWO_PI = 2.0 * PI;

/**
 * @brief The universal settings for MWAV
 */
struct MwavSettings {
  /**
   * @brief Used to determine if a morse call sign should be added to the
   * audio.
   * @details Add a morse call sign before, after, or both before and after the
   * audio.
   */
  enum class CallSignMode { NONE, BEFORE, AFTER, BEFORE_AND_AFTER };

  /**
   * @brief An amateur radio call sign
   * @details Can be left empty if call_sign_mode is set to NONE, but not if
   * using the APRS modulator. Must conform to the FCC's rules for call signs.
   * @warning This may be a problem for international users. If so, please
   * create an issue on GitHub.
   */
  std::string call_sign = "";

  /**
   * @brief The mode for the call sign
   * @see CallSignMode
   */
  MwavSettings::CallSignMode call_sign_mode = MwavSettings::CallSignMode::NONE;
};

/**
 * @brief The base class for all modulators that turn data into audio
 */
class Modulator {
public:
  /**
   * @brief Constructor for the base modulator class
   * @param settings The settings for the modulator
   */
  Modulator(const MwavSettings &settings = MwavSettings())
      : settings_(settings) {}
  virtual ~Modulator() = default;

  /**
   * @brief Write the audio to a file
   * @param filename The name/path of the file to write to
   * @exception MWAVException Throws an exception if the file cannot be opened
   * or if there is no data to write
   */
  void writeToFile(const std::string &filename);

protected:
  /**
   * @brief The settings for the modulator
   */
  const MwavSettings settings_;

  /**
   * @brief Buffer for the audio data, kept generic so it can be written to
   * either a WAV file or to PulseAudio
   */
  std::vector<int16_t> audio_data_ = {};
};

/**
 * @brief The base class for all demodulators that turn audio into data
 */
class Demodulator {
public:
  Demodulator() = default;
  virtual ~Demodulator() = default;
};

} // namespace mwav

#endif /* MWAV_HPP_ */