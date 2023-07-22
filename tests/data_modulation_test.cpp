#include "gtest/gtest.h"
#include <string>

#include <data_modulation.hpp>
#include <wav_gen.hpp>

const std::string kOutFilePath = "test.wav";

TEST(DataModulation, EncodeStringBpsk125) {
  mwav::data::Mode mode = mwav::data::Mode::BPSK_125;
  std::string input = "Hello World!";
  mwav::data::encodeString(mode, input, kOutFilePath);

  const uint32_t minimum_samples = input.size() * 8 * 48000 / 125;

  wavgen::Reader reader(kOutFilePath);
  uint32_t total_samples = reader.getNumSamples();

  EXPECT_GE(total_samples, minimum_samples);
}