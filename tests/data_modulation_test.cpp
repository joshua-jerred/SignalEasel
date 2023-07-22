#include "gtest/gtest.h"
#include <string>

#include <data_modes.hpp>
#include <wav_gen.hpp>

const std::string kOutFilePath = "test.wav";

TEST(DataModulation, EncodeAndDecodeAsciiAFSK) {
  mwav::data::Mode mode = mwav::data::Mode::AFSK1200;
  std::string input = "UUUHello World!UUU";
  const uint32_t minimum_samples = input.size() * 8 * 48000 / 125;

  mwav::data::encodeString(mode, input, kOutFilePath);

  wavgen::Reader reader(kOutFilePath);
  uint32_t total_samples = reader.getNumSamples();
  EXPECT_GE(total_samples, minimum_samples);

  std::string output;
  mwav::data::decodeString(mode, kOutFilePath, output);
  EXPECT_EQ(input, output);
}