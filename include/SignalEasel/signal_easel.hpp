/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   signal_easel.hpp
 * @date   2023-12-04
 * @brief  The main header file for the SignalEasel library
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIG_EASEL_HPP_
#define SIG_EASEL_HPP_

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

/**
 * @brief The namespace used for the MWAV library
 */
namespace signal_easel {

class Exception : public std::exception {
public:
  enum class Id {
    FILE_OPEN_ERROR,
    NO_DATA_TO_WRITE,
    INVALID_CALL_SIGN,
    INVALID_CALL_SIGN_MODE
  };

  static std::string idToString(Id exception_id) {
    switch (exception_id) {
    case Id::FILE_OPEN_ERROR:
      return "File open error";
    case Id::NO_DATA_TO_WRITE:
      return "No data to write";
    case Id::INVALID_CALL_SIGN:
      return "Invalid call sign";
    case Id::INVALID_CALL_SIGN_MODE:
      return "Invalid call sign mode";
    default:
      return "Unknown error";
    }
  };

  Exception(const Exception::Id &exception_id)
      : id_(exception_id), exception_string_(idToString(exception_id)) {}
  virtual const char *what() const throw() { return exception_string_.c_str(); }

private:
  Exception::Id id_;
  std::string exception_string_;
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
 * @brief The maximum size of the audio buffer (5 minutes)
 */
inline constexpr uint32_t MAX_AUDIO_BUFFER_SIZE = AUDIO_SAMPLE_RATE * 60 * 5;

/**
 * @brief Constant for PI used throughout the project
 */
inline constexpr double PI_VAL = 3.14159265358979323846;

/**
 * @brief Constant for 2*PI used throughout the project
 */
inline constexpr double TWO_PI_VAL = 2.0 * PI_VAL;

/**
 * @brief The universal settings for MWAV
 */
struct GlobalSettings {
  /**
   * @brief Used to determine if a morse call sign should be added to the
   * audio.
   * @details Add a morse call sign before, after, or both before and after
   * the audio.
   */
  enum class CallSignMode { NONE, BEFORE, AFTER, BEFORE_AND_AFTER };

  /**
   * @brief An amateur radio call sign
   * @details Can be left empty if call_sign_mode is set to NONE, but not if
   * using the APRS modulator. Must conform to the FCC's rules for call signs.
   * @warning This may be a problem for international users. If so, please
   * create an issue on GitHub.
   */
  std::string call_sign;

  /**
   * @brief The mode for the call sign
   * @see CallSignMode
   */
  GlobalSettings::CallSignMode call_sign_mode =
      GlobalSettings::CallSignMode::NONE;
};

/**
 * @brief The abstract base class for all modulators that turn data into audio
 */
class Modulator {
public:
  /**
   * @brief Constructor for the base modulator class
   * @param settings The settings for the modulator
   */
  Modulator(const GlobalSettings &settings = GlobalSettings())
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
  GlobalSettings settings_;

  bool addAudioSample(int16_t sample) {

    audio_data_.push_back(sample);
    return true;
  }

private:
  /**
   * @brief Buffer for the audio data, kept generic so it can be written to
   * either a WAV file or to PulseAudio
   */
  std::vector<int16_t> audio_data_ = {};
};

/**
 * @brief Another abstract base class for modulators that turn text/binary data
 * into audio
 */
class DataModulator : public Modulator {

  /**
   * @brief The base class for all demodulators that turn audio into data
   */
  class Demodulator {
  public:
    Demodulator() = default;
    virtual ~Demodulator() = default;
  };

} // namespace signal_easel

#endif /* MWAV_HPP_ */