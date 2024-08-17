#include "gtest/gtest.h"

#include <SignalEasel/aprs.hpp>
#include <fstream>

TEST(Aprs, EncodeAndDecodeMessagePacket) {
  const std::string kOutFilePath = "aprs_message_test.wav";

  signal_easel::aprs::Modulator modulator;

  signal_easel::aprs::MessagePacket message_packet;
  message_packet.source_address = "TSTCLL";
  message_packet.source_ssid = 11;
  message_packet.addressee = "TSTCLL-11";
  message_packet.message = "Hello World!";
  message_packet.message_id = "1";

  modulator.encode(message_packet);
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

TEST(Aprs, EncodeAndDecodePositionPacket) {
  const std::string kOutFilePath = "aprs_position_test.wav";

  signal_easel::aprs::Modulator modulator;

  signal_easel::aprs::PositionPacket position_packet;
  position_packet.source_address = "TSTCLL";
  position_packet.source_ssid = 11;
  position_packet.time_code = "000000";
  position_packet.altitude = 1000;
  position_packet.course = 200;
  position_packet.speed = 100;
  position_packet.latitude = 40.0;
  position_packet.longitude = -105.0;

  modulator.encode(position_packet);
  modulator.writeToFile(kOutFilePath);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(kOutFilePath);
  demodulator.processAudioBuffer();
  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(), signal_easel::aprs::Packet::Type::POSITION);

  signal_easel::aprs::PositionPacket decoded_position_packet;
  EXPECT_TRUE(demodulator.parsePositionPacket(decoded_position_packet));

  EXPECT_EQ(decoded_position_packet.time_code, position_packet.time_code);
  EXPECT_EQ(decoded_position_packet.altitude, position_packet.altitude);
  EXPECT_EQ(decoded_position_packet.course, position_packet.course);
  EXPECT_NEAR(decoded_position_packet.speed, position_packet.speed, 0.5);
  EXPECT_NEAR(decoded_position_packet.latitude, position_packet.latitude,
              0.001);
  EXPECT_NEAR(decoded_position_packet.longitude, position_packet.longitude,
              0.001);
}

TEST(Aprs, EncodeAndDecodeExperimentalPacket) {
  const std::string OUT_FILE_PATH = "aprs_experimental_test.wav";

  signal_easel::aprs::Modulator modulator;

  signal_easel::aprs::ExperimentalPacket experimental_packet;
  experimental_packet.source_address = "TSTCLL";
  experimental_packet.source_ssid = 11;
  experimental_packet.packet_type_char = 'z';
  std::string data_str = "experimentaldatatest";
  experimental_packet.setStringData(data_str);

  modulator.encode(experimental_packet);
  modulator.writeToFile(OUT_FILE_PATH);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(OUT_FILE_PATH);
  demodulator.processAudioBuffer();
  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(),
            signal_easel::aprs::Packet::Type::EXPERIMENTAL);

  signal_easel::aprs::ExperimentalPacket decoded_experimental_packet;
  EXPECT_TRUE(demodulator.parseExperimentalPacket(decoded_experimental_packet));

  EXPECT_EQ(decoded_experimental_packet.packet_type_char,
            experimental_packet.packet_type_char);
  EXPECT_EQ(decoded_experimental_packet.getStringData(), data_str);
}

TEST(Aprs, DecodeReal) {
  const std::string kInputFile = "aprs_real.wav";

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(kInputFile);
  demodulator.processAudioBuffer();
  EXPECT_TRUE(demodulator.lookForAx25Packet());
  demodulator.printFrame();
}

// TEST(Aprs, Telemetry) {
//   signal_easel::aprs::Telemetry packet;
//   packet.sequence_number = "010";
//   packet.destination_address = "APRS";
//   packet.a1.value = 1;
//   packet.a2.value = 2;
//   packet.a3.value = 3;
//   packet.a4.value = 4;
//   packet.a5.value = 5;
//   packet.d1.value = true;
//   packet.d2.value = true;
//   packet.d3.value = false;
//   packet.d4.value = false;
//   packet.d5.value = true;
//   packet.d6.value = true;
//   packet.d7.value = true;
//   packet.d8.value = true;

//   auto vec =
//       packet.encode(signal_easel::aprs::TelemetryPacketType::DATA_REPORT);
//   std::string str(vec.begin(), vec.end());
//   EXPECT_EQ(str, "T#010,001,002,003,004,005,11001111");

//   packet.a1.name = "A1";
//   packet.a2.name = "A2";
//   packet.a3.name = "A3";
//   packet.a4.name = "A4";
//   packet.a5.name = "A5";
//   packet.d1.name = "D1";
//   packet.d2.name = "D2";
//   packet.d3.name = "D3";
//   packet.d4.name = "D4";
//   packet.d5.name = "D5";
//   packet.d6.name = "D6";
//   packet.d7.name = "D7";
//   packet.d8.name = "D8";

//   vec = packet.encode(signal_easel::aprs::TelemetryPacketType::PARAM_NAME);
//   str = std::string(vec.begin(), vec.end());
//   EXPECT_EQ(str, "PARM.A1,A2,A3,A4,A5,D1,D2,D3,D4,D5,D6,D7,D8");

//   packet.a1.unit = "V";
//   packet.a2.unit = "V";
//   packet.a3.unit = "V";
//   packet.a4.unit = "V";
//   packet.a5.unit = "V";
//   packet.d1.unit = "V";
//   packet.d2.unit = "V";
//   packet.d3.unit = "V";
//   packet.d4.unit = "V";
//   packet.d5.unit = "V";
//   packet.d6.unit = "V";
//   packet.d7.unit = "V";
//   packet.d8.unit = "V";

//   vec = packet.encode(signal_easel::aprs::TelemetryPacketType::PARAM_UNIT);
//   str = std::string(vec.begin(), vec.end());
//   EXPECT_EQ(str, "UNIT.V,V,V,V,V,V,V,V,V,V,V,V,V");

//   packet.a1.coef_a = "1";
//   packet.a1.coef_b = "2.0";
//   packet.a1.coef_c = "3.0";
//   packet.a2.coef_a = "0.09";
//   packet.a2.coef_b = "0.8";
//   packet.a2.coef_c = "0.0";
//   packet.a3.coef_a = "0.09";
//   packet.a3.coef_b = "0.8";
//   packet.a3.coef_c = "0.0";
//   packet.a4.coef_a = "0.09";
//   packet.a4.coef_b = "0.4";
//   packet.a4.coef_c = "0.0";
//   packet.a5.coef_a = "0.09";
//   packet.a5.coef_b = "0.4";
//   packet.a5.coef_c = "1";

//   vec = packet.encode(signal_easel::aprs::TelemetryPacketType::PARAM_COEF);
//   str = std::string(vec.begin(), vec.end());
//   EXPECT_EQ(str,
//             "EQNS.1,2.0,3.0,0.09,0.8,0.0,0.09,0.8,0.0,0.09,0.4,0.0,0.09,0.4,1");

//   packet.project_title = ",Test Project";
//   packet.d1.sense = true;
//   packet.d2.sense = true;
//   packet.d3.sense = true;
//   packet.d4.sense = true;
//   packet.d5.sense = false;
//   packet.d6.sense = false;
//   packet.d7.sense = false;
//   packet.d8.sense = false;

//   vec = packet.encode(
//       signal_easel::aprs::TelemetryPacketType::BIT_SENSE_PROJ_NAME);
//   str = std::string(vec.begin(), vec.end());
//   EXPECT_EQ(str, "BITS.11110000,Test Project");
// }