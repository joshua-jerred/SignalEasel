/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   modulator.hpp
 * @date   2023-12-10
 * @brief  Modulator base class
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#ifndef SIGNAL_EASEL_MODULATOR_HPP_
#define SIGNAL_EASEL_MODULATOR_HPP_

#include <cstdint>
#include <vector>

#include <SignalEasel/settings.hpp>

namespace signal_easel {

/**
 * @brief The abstract base class for all modulators that turn data into audio
 */
class Modulator {
public:
  /**
   * @brief Constructor for the base modulator class
   * @param settings The settings for the modulator
   */
  Modulator(Settings settings = Settings());
  virtual ~Modulator() = default;

  /**
   * @brief Clears the audio buffer. Adds the call sign if it's in the before
   * or before_and_after modes
   */
  void clearBuffer();

  /**
   * @brief Write the audio to a file
   * @param filename The name/path of the file to write to
   * @exception MWAVException Throws an exception if the file cannot be opened
   * or if there is no data to write
   */
  void writeToFile(const std::string &filename);

  /**
   * @brief Write the audio to the default PulseAudio output device
   */
  void writeToPulseAudio();

protected:
  /**
   * @brief Add a PCM sample to the audio buffer
   * @param sample The PCM sample to add
   */
  void addAudioSample(int16_t sample) { audio_buffer_.push_back(sample); }

  /**
   * @brief Add a PCM sample to the audio buffer
   * @param sample The PCM sample to add - -1.0 to 1.0
   */
  void addAudioSampleDouble(double sample) {
    audio_buffer_.push_back(static_cast<int16_t>(sample * 32767));
  }

  /**
   * @brief Add silence to the audio buffer
   * @param duration_in_samples
   */
  void addSilence(uint32_t duration_in_samples) {
    for (uint32_t i = 0; i < duration_in_samples; i++) {
      addAudioSample(0);
    }
  }

  /**
   * @brief Add a sine wave to the audio buffer
   *
   * @param frequency The frequency of the sine wave in Hz
   * @param num_samples The number of samples to add
   */
  void addSineWave(uint16_t frequency, uint16_t num_samples);

  Settings settings_;

  std::vector<int16_t> audio_buffer_ = {};

private:
  /**
   * @brief Add morse code to the output audio
   * @param input_string the string to convert to morse code
   */
  void addMorseCode(const std::string &input_string);

  /**
   * @brief Used with addSineWave to keep a continuous phase between calls
   */
  double sine_wave_phase_ = 0.0;
};

/**
 * @brief Another abstract base class for modulators that turn text/binary data
 * into audio
 */
class DataModulator : public Modulator {
public:
  DataModulator(const Settings &settings = Settings()) : Modulator(settings) {}
  virtual ~DataModulator() = default;

  /**
   * @brief Add data to the audio buffer
   * @param data The data to add
   */
  void addBytes(const std::vector<uint8_t> &data);

  /**
   * @brief Add string data to the audio buffer
   * @param data The string to add
   */
  void addString(const std::string &data);

protected:
  /**
   * @brief This is overridden by the child classes to encode the data that
   * is passed in by addBytes and addString
   * @param data The data to encode and add to the audio buffer
   */
  virtual void encodeBytes(const std::vector<uint8_t> &data) = 0;
};

} // namespace signal_easel

#endif /* SIGNAL_EASEL_MODULATOR_HPP_ */