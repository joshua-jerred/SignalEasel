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

Modulator::Modulator(GlobalSettings settings) : settings_(std::move(settings)) {
  if (settings_.call_sign_mode != GlobalSettings::CallSignMode::NONE &&
      !isCallSignValid(settings_.call_sign)) {
    throw Exception(Exception::Id::INVALID_CALL_SIGN);
  }
}

void Modulator::writeToFile(const std::string &filename) {
  if (audio_buffer_.empty()) {
    throw Exception(Exception::Id::NO_DATA_TO_WRITE);
  }

  if (settings_.call_sign_mode != GlobalSettings::CallSignMode::NONE &&
      !isCallSignValid(settings_.call_sign)) {
    throw Exception(Exception::Id::INVALID_CALL_SIGN);
  }

  try {
    wavgen::Writer wav_file(filename);

    if (settings_.call_sign_mode == GlobalSettings::CallSignMode::BEFORE ||
        settings_.call_sign_mode ==
            GlobalSettings::CallSignMode::BEFORE_AND_AFTER) {
      addMorseCode(settings_.call_sign);
      addSilence(settings_.call_sign_pause_seconds * AUDIO_SAMPLE_RATE);
    }

    for (const auto &sample : audio_buffer_) {
      wav_file.addSample(sample);
    }

    if (settings_.call_sign_mode == GlobalSettings::CallSignMode::AFTER ||
        settings_.call_sign_mode ==
            GlobalSettings::CallSignMode::BEFORE_AND_AFTER) {
      addSilence(settings_.call_sign_pause_seconds * AUDIO_SAMPLE_RATE);
      addMorseCode(settings_.call_sign);
    }

  } catch (const std::exception &e) {
    throw Exception(Exception::Id::FILE_OPEN_ERROR);
  }
}

void Modulator::addSineWave(uint16_t frequency, uint16_t num_samples) {
  if (frequency == 0 || num_samples == 0) {
    return;
  }

  const double delta = TWO_PI_VAL * frequency / AUDIO_SAMPLE_RATE;

  for (uint16_t i = 0; i < num_samples; i++) {
    const int16_t sample = settings_.amplitude * std::sin(sine_wave_phase_);
    addAudioSample(sample);
    sine_wave_phase_ += delta;
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