#include "gtest/gtest.h"

#include <SignalEasel/aprs.hpp>
#include <fstream>

TEST(Aprs, EncodeAndDecodeMessagePacket) {
  const std::string kOutFilePath = "aprs_message_test.wav";

  signal_easel::aprs::Packet packet;
  packet.source_address = "TSTCLL";
  packet.source_ssid = 11;

  signal_easel::aprs::Modulator modulator;
  modulator.setBasePacket(packet);

  signal_easel::aprs::MessagePacket message_packet;
  message_packet.addressee = "TSTCLL-11";
  message_packet.message = "Hello World!";
  message_packet.message_id = "1";

  modulator.encodeMessagePacket(message_packet);
  modulator.writeToFile(kOutFilePath);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(kOutFilePath);
  demodulator.processAudioBuffer();
  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(), signal_easel::aprs::Packet::Type::MESSAGE);

  signal_easel::aprs::MessagePacket decoded_message_packet;
  EXPECT_TRUE(demodulator.parseMessagePacket(decoded_message_packet));

  EXPECT_EQ(decoded_message_packet.addressee, message_packet.addressee);
  EXPECT_EQ(decoded_message_packet.message, message_packet.message);
  EXPECT_EQ(decoded_message_packet.message_id, message_packet.message_id);
}

TEST(Aprs, EncodeAndDecodeLocationPacket) {
  // const std::string kOutFilePath = "aprs_basic_test.wav";

  // signal_easel::AprsPacket packet;
  // packet.source_address = "TEST";

  // signal_easel::AprsModulator modulator;
  // modulator.setBasePacket(packet);

  // signal_easel::AprsPositionPacket position_packet;
  // position_packet.time_code = "123456";
  // position_packet.latitude = 40.0;

  // modulator.encodePositionPacket(position_packet);
  // modulator.writeToFile(kOutFilePath);

  // signal_easel::AprsDemodulator demodulator;
  // demodulator.loadAudioFromFile(kOutFilePath);
  // demodulator.processAudioBuffer();
  // EXPECT_TRUE(demodulator.lookForAx25Packet());
}

// TEST(Aprs, DecodeMessagePacket) {
//   const std::string kInputFile = "aprs_message.wav";

//   signal_easel::AprsDemodulator demodulator;
//   demodulator.loadAudioFromFile(kInputFile);
//   demodulator.processAudioBuffer();
//   EXPECT_TRUE(demodulator.lookForAx25Packet());
// }

TEST(Aprs, DecodeReal) {
  const std::string kInputFile = "aprs_real.wav";

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(kInputFile);
  demodulator.processAudioBuffer();
  EXPECT_TRUE(demodulator.lookForAx25Packet());
  demodulator.printFrame();
}

TEST(Aprs, Telemetry) {}