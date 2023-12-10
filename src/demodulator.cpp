/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   demodulator.cpp
 * @date   2023-12-05
 * @brief  The demodulator class implementation
 *
 * =*=======================*=
 * @copyright  2023 Joshua Jerred
 * @license    GNU GPLv3
 */

#include <iostream>
#include <wav_gen.hpp>

#include <SignalEasel/demodulator.hpp>
#include <SignalEasel/exception.hpp>

namespace signal_easel {

void Demodulator::loadAudioFromFile(const std::string &file_name) {
  try {
    wavgen::Reader reader(file_name);
    std::vector<int16_t> samples;
    reader.getAllSamples(samples);

    std::copy(samples.begin(), samples.end(),
              std::back_inserter(audio_buffer_));
  } catch (const std::exception &e) {
    throw Exception(Exception::Id::FILE_OPEN_ERROR);
  }
}

} // namespace signal_easel
