#include "gtest/gtest.h"

#include <SignalEasel/aprs.hpp>
#include <fstream>

TEST(Aprs, LocationPacket) {
  const std::string kOutFilePath = "aprs_basic_test.wav";

  signal_easel::AprsPacket packet;
  packet.source_address = "KD9GDC";

  signal_easel::AprsModulator modulator;
  modulator.setBasePacket(packet);

  signal_easel::AprsPositionPacket position_packet;
  position_packet.time_code = "123456";
  position_packet.latitude = 40.0;

  modulator.encodePositionPacket(position_packet);
  modulator.writeToFile(kOutFilePath);

  // signal_easel::AprsDemodulator demodulator;
  // demodulator.loadAudioFromFile(kOutFilePath);
  // demodulator.processAudioBuffer();
  // EXPECT_TRUE(demodulator.lookForAx25Packet());
}

TEST(Aprs, AprsMessageDecode) {
  const std::string kInputFile = "aprs_message.wav";

  signal_easel::AprsDemodulator demodulator;
  demodulator.loadAudioFromFile(kInputFile);
  demodulator.processAudioBuffer();
  EXPECT_TRUE(demodulator.lookForAx25Packet());
}