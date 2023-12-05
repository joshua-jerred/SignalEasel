#include "gtest/gtest.h"

#include <string>

#include <SignalEasel/afsk.hpp>

/**
 * @brief Encodes a string into an AFSK1200 signal/WAV file and then decodes it.
 */
TEST(Afsk, EncodeDecodeBasicStringToWav) {
  const std::string kInputString = "Hello World! How are you today?";
  const std::string kOutFilePath = "afsk_test_EncodeDecodeBasicStringToWav.wav";

  signal_easel::AfskModulator modulator;
  modulator.addString(kInputString);
  modulator.writeToFile(kOutFilePath);

  signal_easel::AfskDemodulator demodulator;
  demodulator.loadAudioFromFile(kOutFilePath);
  demodulator.processAudioBuffer();

  std::string output;
  EXPECT_EQ(demodulator.lookForString(output),
            signal_easel::AfskDemodulator::AsciiResult::SUCCESS);
  EXPECT_STREQ(kInputString.c_str(), output.c_str());

  // mwav::data::encodeString(kMode, input, kOutFilePath);
  // mwav::data::decodeString(kMode, kOutFilePath, output);
  // EXPECT_STREQ(input.c_str(), output.c_str());

  // wavgen::Reader reader(kOutFilePath);
  // const auto total_samples = reader.getNumSamples();
  // const auto minimum_samples = input.size() * 8;
  // EXPECT_GE(total_samples, minimum_samples);
}

/**
 * @brief Pre-prepared WAV file with 20 samples added to the start of the file.
 * @details 20 samples is enough to throw the signal off by 1/2 of a bit. This
 * should test the ability of the demodulator to sync to the signal.
 */
TEST(Afsk, DecodeWithSignalSkew) {
  const std::string kInputFile = "afsk_timing_skew.wav";
  std::string output;
  // mwav::data::decodeString(kMode, kInputFile, output);

  // signal_easel::AfskDemodulator demodulator;
  // demodulator.loadAudioFromFile(kInputFile);
  // demodulator.processAudioBuffer();

  // EXPECT_EQ(demodulator.lookForString(output),
  //           signal_easel::AfskDemodulator::AsciiResult::SUCCESS);
  // EXPECT_STREQ("Hello World!", output.c_str());
  // std::cout << output << std::endl;
}

// TEST(Afsk, WhiteNoise) {
//   const std::string kInputFile = "white_noise.wav";
//   std::string output;
//   mwav::data::decodeString(kMode, kInputFile, output);

//   std::cout << output << std::endl;
// }