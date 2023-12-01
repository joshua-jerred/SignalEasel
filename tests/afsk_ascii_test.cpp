#include "gtest/gtest.h"
#include <string>

#include <data_modes.hpp>
#include <wav_gen.hpp>

const std::string kOutFilePath = "test.wav";

TEST(DataModulation, EncodeAndDecodeAsciiAFSK) {
  mwav::data::Mode mode = mwav::data::Mode::AFSK1200;

  std::string input = "UUUHello World!UUU\x04";
  std::string output;

  mwav::data::encodeString(mode, input, kOutFilePath);
  mwav::data::decodeString(mode, kOutFilePath, output);
  EXPECT_STREQ(input.c_str(), output.c_str());

  wavgen::Reader reader(kOutFilePath);
  const auto total_samples = reader.getNumSamples();
  const auto minimum_samples = input.size() * 8;
  EXPECT_GE(total_samples, minimum_samples);
}