/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   modulator.cpp
 * @date   2023-12-04
 * @brief  The abstract modulator class implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <cmath>

#include <wav_gen.hpp>

#include <SignalEasel/signal_easel.hpp>

namespace signal_easel {

Modulator::Modulator(Settings settings) : SignalEasel(std::move(settings)) {
  clearBuffer();
}

void Modulator::clearBuffer() {
  audio_buffer_.clear();
  sine_wave_phase_ = 0;

  if (settings_.call_sign_mode != Settings::CallSignMode::NONE &&
      !isCallSignValid(settings_.call_sign)) {
    throw Exception(Exception::Id::INVALID_CALL_SIGN);
  }

  if (settings_.call_sign_mode == Settings::CallSignMode::BEFORE ||
      settings_.call_sign_mode == Settings::CallSignMode::BEFORE_AND_AFTER) {
    addMorseCode(settings_.call_sign);
    addSilence(static_cast<uint32_t>(settings_.call_sign_pause_seconds *
                                     AUDIO_SAMPLE_RATE));
  }
}

void Modulator::writeToFile(const std::string &filename) {
  if (audio_buffer_.empty()) {
    throw Exception(Exception::Id::NO_DATA_TO_WRITE);
  }

  if (settings_.call_sign_mode != Settings::CallSignMode::NONE &&
      !isCallSignValid(settings_.call_sign)) {
    throw Exception(Exception::Id::INVALID_CALL_SIGN);
  }

  try {
    wavgen::Writer wav_file(filename);

    for (const auto &sample : audio_buffer_) {
      wav_file.addSample(sample);
    }

    if (settings_.call_sign_mode == Settings::CallSignMode::AFTER ||
        settings_.call_sign_mode == Settings::CallSignMode::BEFORE_AND_AFTER) {
      addSilence(static_cast<uint32_t>(settings_.call_sign_pause_seconds *
                                       AUDIO_SAMPLE_RATE));
      addMorseCode(settings_.call_sign);
    }

    wav_file.done();
  } catch (const std::exception &e) {
    throw Exception(Exception::Id::FILE_OPEN_ERROR);
  }
}

void Modulator::addSineWave(uint16_t frequency, uint16_t num_samples) {
  if (frequency == 0 || num_samples == 0) {
    return;
  }

  const double k_delta = TWO_PI_VAL * frequency / AUDIO_SAMPLE_RATE;

  for (uint16_t i = 0; i < num_samples; i++) {
    int16_t sample = static_cast<int16_t>(
        settings_.amplitude * std::sin(sine_wave_phase_) * MAX_SAMPLE_VALUE);
    addAudioSample(sample);
    sine_wave_phase_ += k_delta;
  }
}

void DataModulator::addBytes(const std::vector<uint8_t> &data) {
  if (data.empty()) {
    return;
  }
  encodeBytes(data);
}

void DataModulator::addString(const std::string &data) {
  if (data.empty()) {
    return;
  }
  encodeBytes(std::vector<uint8_t>(data.begin(), data.end()));
}

} // namespace signal_easel