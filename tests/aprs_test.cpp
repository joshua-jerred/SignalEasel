#include "gtest/gtest.h"

#include <SignalEasel/aprs.hpp>

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

  // signal_easel::AfskModulator modulator;
  // modulator.addString(kInputString);
  // modulator.writeToFile(kOutFilePath);

  signal_easel::AfskDemodulator demodulator;
  demodulator.loadAudioFromFile(kOutFilePath);
  demodulator.processAudioBuffer();
  demodulator.
}

TEST(Aprs, AprsMessageDecode) { signal_easel::AprsDemodulator demodulator; }