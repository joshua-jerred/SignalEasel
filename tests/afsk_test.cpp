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

  signal_easel::AfskDemodulator demodulator;
  demodulator.loadAudioFromFile(kInputFile);
  auto res = demodulator.processAudioBuffer();
  EXPECT_GT(res.snr, 0.0);

  EXPECT_EQ(demodulator.lookForString(output),
            signal_easel::AfskDemodulator::AsciiResult::SUCCESS);
  EXPECT_STREQ("Hello World!", output.c_str());
  // std::cout << output << std::endl;
}

TEST(Afsk, WhiteNoise) {
  const std::string kInputFile = "white_noise.wav";
  std::string output;
  //   mwav::data::decodeString(kMode, kInputFile, output);

  signal_easel::AfskDemodulator demodulator;
  demodulator.loadAudioFromFile(kInputFile);
  auto res = demodulator.processAudioBuffer();

  EXPECT_LE(res.snr, 0.0);
  EXPECT_NE(demodulator.lookForString(output),
            signal_easel::AfskDemodulator::AsciiResult::SUCCESS);
}