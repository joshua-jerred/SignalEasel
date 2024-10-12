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

  // Verify that the generic fields were also decoded correctly
  EXPECT_EQ(experimental_packet.source_address,
            decoded_experimental_packet.source_address);
  EXPECT_EQ(experimental_packet.source_ssid,
            decoded_experimental_packet.source_ssid);

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

TEST(Aprs, EncodeAndDecodeTelemetryDataReport) {
  const std::string OUT_FILE_PATH = "aprs_telemetry_data_report_test.wav";

  namespace tel = signal_easel::aprs::telemetry;

  tel::TelemetryData in_data;
  in_data.setSequenceNumber(100);
  in_data.getAnalog(tel::AnalogParameter::Id::A1).setRawValue(2);
  in_data.getAnalog(tel::AnalogParameter::Id::A2).setRawValue(10);
  in_data.getAnalog(tel::AnalogParameter::Id::A3).setRawValue(100);
  in_data.getAnalog(tel::AnalogParameter::Id::A4).setRawValue(255);
  in_data.getAnalog(tel::AnalogParameter::Id::A5).setRawValue(99);
  in_data.getDigital(tel::DigitalParameter::Id::B1).setValue(true);
  in_data.getDigital(tel::DigitalParameter::Id::B2).setValue(false);
  in_data.getDigital(tel::DigitalParameter::Id::B3).setValue(false);
  in_data.getDigital(tel::DigitalParameter::Id::B4).setValue(false);
  in_data.getDigital(tel::DigitalParameter::Id::B5).setValue(false);
  in_data.getDigital(tel::DigitalParameter::Id::B6).setValue(true);
  in_data.getDigital(tel::DigitalParameter::Id::B7).setValue(false);
  in_data.getDigital(tel::DigitalParameter::Id::B8).setValue(true);

  signal_easel::aprs::TelemetryPacket encode_packet{in_data};
  encode_packet.telemetry_type =
      signal_easel::aprs::Packet::Type::TELEMETRY_DATA_REPORT;
  encode_packet.source_address = "TSTCLL";
  encode_packet.source_ssid = 11;

  signal_easel::aprs::Modulator modulator;
  modulator.encode(encode_packet);
  modulator.writeToFile(OUT_FILE_PATH);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(OUT_FILE_PATH);
  demodulator.processAudioBuffer();

  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(),
            signal_easel::aprs::Packet::Type::TELEMETRY_DATA_REPORT);

  // -- Decode --
  signal_easel::aprs::TelemetryPacket decoded_packet;
  EXPECT_TRUE(demodulator.parseTelemetryPacket(decoded_packet));
  EXPECT_EQ(signal_easel::aprs::Packet::Type::TELEMETRY_DATA_REPORT,
            decoded_packet.telemetry_type);
  auto &out_data = decoded_packet.telemetry_data;

  // Check generics
  EXPECT_EQ(encode_packet.source_address, decoded_packet.source_address);
  EXPECT_EQ(encode_packet.source_ssid, decoded_packet.source_ssid);
  EXPECT_EQ(encode_packet.destination_address,
            decoded_packet.destination_address);
  EXPECT_EQ(encode_packet.destination_ssid, decoded_packet.destination_ssid);

  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A1).getValue(),
            out_data.getAnalog(tel::AnalogParameter::Id::A1).getValue());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A2).getValue(),
            out_data.getAnalog(tel::AnalogParameter::Id::A2).getValue());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A3).getValue(),
            out_data.getAnalog(tel::AnalogParameter::Id::A3).getValue());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A4).getValue(),
            out_data.getAnalog(tel::AnalogParameter::Id::A4).getValue());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A5).getValue(),
            out_data.getAnalog(tel::AnalogParameter::Id::A5).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B1).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B1).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B2).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B2).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B3).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B3).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B4).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B4).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B5).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B5).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B6).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B6).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B7).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B7).getValue());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B8).getValue(),
            out_data.getDigital(tel::DigitalParameter::Id::B8).getValue());
}

TEST(Aprs, EncodeAndDecodeTelemetryBitSense) {
  const std::string OUT_FILE_PATH = "aprs_telemetry_bit_sense_test.wav";

  namespace tel = signal_easel::aprs::telemetry;

  tel::TelemetryData in_data;
  in_data.setTelemetryStationAddress("TST-1");
  in_data.setProjectTitle("project 1234");
  in_data.getDigital(tel::DigitalParameter::Id::B1).setBitSense(true);
  in_data.getDigital(tel::DigitalParameter::Id::B2).setBitSense(false);
  in_data.getDigital(tel::DigitalParameter::Id::B3).setBitSense(true);
  in_data.getDigital(tel::DigitalParameter::Id::B4).setBitSense(false);
  in_data.getDigital(tel::DigitalParameter::Id::B5).setBitSense(false);
  in_data.getDigital(tel::DigitalParameter::Id::B6).setBitSense(true);
  in_data.getDigital(tel::DigitalParameter::Id::B7).setBitSense(false);
  in_data.getDigital(tel::DigitalParameter::Id::B8).setBitSense(false);

  signal_easel::aprs::TelemetryPacket encode_packet{in_data};
  encode_packet.telemetry_type =
      signal_easel::aprs::Packet::Type::TELEMETRY_BIT_SENSE_PROJ_NAME;
  encode_packet.source_address = "TSTCLL";
  encode_packet.source_ssid = 11;

  signal_easel::aprs::Modulator modulator;
  modulator.encode(encode_packet);
  modulator.writeToFile(OUT_FILE_PATH);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(OUT_FILE_PATH);
  demodulator.processAudioBuffer();

  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(),
            signal_easel::aprs::Packet::Type::TELEMETRY_BIT_SENSE_PROJ_NAME);

  // -- Decode --
  signal_easel::aprs::TelemetryPacket decoded_packet;
  EXPECT_TRUE(demodulator.parseTelemetryPacket(decoded_packet));
  EXPECT_EQ(signal_easel::aprs::Packet::Type::TELEMETRY_BIT_SENSE_PROJ_NAME,
            decoded_packet.telemetry_type);

  // Check generics
  EXPECT_EQ(encode_packet.source_address, decoded_packet.source_address);
  EXPECT_EQ(encode_packet.source_ssid, decoded_packet.source_ssid);
  EXPECT_EQ(encode_packet.destination_address,
            decoded_packet.destination_address);
  EXPECT_EQ(encode_packet.destination_ssid, decoded_packet.destination_ssid);

  // Check specifics
  auto &out_data = decoded_packet.telemetry_data;

  EXPECT_EQ(in_data.getTelemetryStationAddress(),
            out_data.getTelemetryStationAddress());
  EXPECT_EQ(in_data.getProjectTitle(), out_data.getProjectTitle());

  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B1).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B1).getBitSense());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B2).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B2).getBitSense());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B3).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B3).getBitSense());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B4).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B4).getBitSense());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B5).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B5).getBitSense());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B6).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B6).getBitSense());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B7).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B7).getBitSense());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B8).getBitSense(),
            out_data.getDigital(tel::DigitalParameter::Id::B8).getBitSense());
}

TEST(Aprs, EncodeAndDecodeTelemetryParameterName) {
  const std::string OUT_FILE_PATH = "aprs_telemetry_parameter_name_test.wav";

  namespace tel = signal_easel::aprs::telemetry;

  tel::TelemetryData in_data;
  in_data.setTelemetryStationAddress("TST-1");

  in_data.getAnalog(tel::AnalogParameter::Id::A1).setName("aaaaaaa");
  in_data.getAnalog(tel::AnalogParameter::Id::A2).setName("aaaaaa");
  in_data.getAnalog(tel::AnalogParameter::Id::A3).setName("aaaaa");
  in_data.getAnalog(tel::AnalogParameter::Id::A4).setName("aaaaa");
  in_data.getAnalog(tel::AnalogParameter::Id::A5).setName("aaaa");
  in_data.getDigital(tel::DigitalParameter::Id::B1).setName("bbbbb");
  in_data.getDigital(tel::DigitalParameter::Id::B2).setName("bbbb");
  in_data.getDigital(tel::DigitalParameter::Id::B3).setName("bbb");
  in_data.getDigital(tel::DigitalParameter::Id::B4).setName("bbb");
  in_data.getDigital(tel::DigitalParameter::Id::B5).setName("bbb");
  in_data.getDigital(tel::DigitalParameter::Id::B6).setName("bb");
  in_data.getDigital(tel::DigitalParameter::Id::B7).setName("bb");
  in_data.getDigital(tel::DigitalParameter::Id::B8).setName("bb");

  signal_easel::aprs::TelemetryPacket encode_packet{in_data};
  encode_packet.telemetry_type =
      signal_easel::aprs::Packet::Type::TELEMETRY_PARAMETER_NAME;
  encode_packet.source_address = "TSTCLL";
  encode_packet.source_ssid = 11;
  encode_packet.destination_address = "APZSEA";
  encode_packet.destination_ssid = 3;

  signal_easel::aprs::Modulator modulator;
  modulator.encode(encode_packet);
  modulator.writeToFile(OUT_FILE_PATH);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(OUT_FILE_PATH);
  demodulator.processAudioBuffer();

  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(),
            signal_easel::aprs::Packet::Type::TELEMETRY_PARAMETER_NAME);

  // -- Decode --
  signal_easel::aprs::TelemetryPacket decoded_packet;
  EXPECT_TRUE(demodulator.parseTelemetryPacket(decoded_packet));
  EXPECT_EQ(signal_easel::aprs::Packet::Type::TELEMETRY_PARAMETER_NAME,
            decoded_packet.telemetry_type);

  // Check generics
  EXPECT_EQ(encode_packet.source_address, decoded_packet.source_address);
  EXPECT_EQ(encode_packet.source_ssid, decoded_packet.source_ssid);
  EXPECT_EQ(encode_packet.destination_address,
            decoded_packet.destination_address);
  EXPECT_EQ(encode_packet.destination_ssid, decoded_packet.destination_ssid);

  // Check specifics
  auto &out_data = decoded_packet.telemetry_data;

  EXPECT_EQ(in_data.getTelemetryStationAddress(),
            out_data.getTelemetryStationAddress());

  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A1).getName(),
            out_data.getAnalog(tel::AnalogParameter::Id::A1).getName());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A2).getName(),
            out_data.getAnalog(tel::AnalogParameter::Id::A2).getName());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A3).getName(),
            out_data.getAnalog(tel::AnalogParameter::Id::A3).getName());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A4).getName(),
            out_data.getAnalog(tel::AnalogParameter::Id::A4).getName());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A5).getName(),
            out_data.getAnalog(tel::AnalogParameter::Id::A5).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B1).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B1).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B2).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B2).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B3).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B3).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B4).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B4).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B5).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B5).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B6).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B6).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B7).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B7).getName());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B8).getName(),
            out_data.getDigital(tel::DigitalParameter::Id::B8).getName());
}

TEST(Aprs, EncodeAndDecodeTelemetryParameterUnitOrLabel) {
  const std::string OUT_FILE_PATH =
      "aprs_telemetry_parameter_unit_or_label_test.wav";

  namespace tel = signal_easel::aprs::telemetry;

  tel::TelemetryData in_data;
  in_data.setTelemetryStationAddress("TST-1");

  in_data.getAnalog(tel::AnalogParameter::Id::A1).setUnitOrLabel("a1");
  in_data.getAnalog(tel::AnalogParameter::Id::A2).setUnitOrLabel("a2");
  in_data.getAnalog(tel::AnalogParameter::Id::A3).setUnitOrLabel("a3");
  in_data.getAnalog(tel::AnalogParameter::Id::A4).setUnitOrLabel("a4");
  in_data.getAnalog(tel::AnalogParameter::Id::A5).setUnitOrLabel("a5");
  in_data.getDigital(tel::DigitalParameter::Id::B1).setUnitOrLabel("b1");
  in_data.getDigital(tel::DigitalParameter::Id::B2).setUnitOrLabel("b2");
  in_data.getDigital(tel::DigitalParameter::Id::B3).setUnitOrLabel("b3");
  in_data.getDigital(tel::DigitalParameter::Id::B4).setUnitOrLabel("b4");
  in_data.getDigital(tel::DigitalParameter::Id::B5).setUnitOrLabel("b5");
  in_data.getDigital(tel::DigitalParameter::Id::B6).setUnitOrLabel("b6");
  in_data.getDigital(tel::DigitalParameter::Id::B7).setUnitOrLabel("b7");
  in_data.getDigital(tel::DigitalParameter::Id::B8).setUnitOrLabel("b8");

  signal_easel::aprs::TelemetryPacket encode_packet{in_data};
  encode_packet.telemetry_type =
      signal_easel::aprs::Packet::Type::TELEMETRY_PARAMETER_UNIT;
  encode_packet.source_address = "TSTCLL";
  encode_packet.source_ssid = 11;
  encode_packet.destination_address = "APZSEA";
  encode_packet.destination_ssid = 3;

  signal_easel::aprs::Modulator modulator;
  modulator.encode(encode_packet);
  modulator.writeToFile(OUT_FILE_PATH);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(OUT_FILE_PATH);
  demodulator.processAudioBuffer();

  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(),
            signal_easel::aprs::Packet::Type::TELEMETRY_PARAMETER_UNIT);

  // -- Decode --
  signal_easel::aprs::TelemetryPacket decoded_packet;
  EXPECT_TRUE(demodulator.parseTelemetryPacket(decoded_packet));
  EXPECT_EQ(signal_easel::aprs::Packet::Type::TELEMETRY_PARAMETER_UNIT,
            decoded_packet.telemetry_type);

  // Check generics
  EXPECT_EQ(encode_packet.source_address, decoded_packet.source_address);
  EXPECT_EQ(encode_packet.source_ssid, decoded_packet.source_ssid);
  EXPECT_EQ(encode_packet.destination_address,
            decoded_packet.destination_address);
  EXPECT_EQ(encode_packet.destination_ssid, decoded_packet.destination_ssid);

  // Check specifics
  auto &out_data = decoded_packet.telemetry_data;

  EXPECT_EQ(in_data.getTelemetryStationAddress(),
            out_data.getTelemetryStationAddress());

  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A1).getLabel(),
            out_data.getAnalog(tel::AnalogParameter::Id::A1).getLabel());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A2).getLabel(),
            out_data.getAnalog(tel::AnalogParameter::Id::A2).getLabel());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A3).getLabel(),
            out_data.getAnalog(tel::AnalogParameter::Id::A3).getLabel());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A4).getLabel(),
            out_data.getAnalog(tel::AnalogParameter::Id::A4).getLabel());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A5).getLabel(),
            out_data.getAnalog(tel::AnalogParameter::Id::A5).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B1).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B1).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B2).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B2).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B3).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B3).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B4).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B4).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B5).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B5).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B6).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B6).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B7).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B7).getLabel());
  EXPECT_EQ(in_data.getDigital(tel::DigitalParameter::Id::B8).getLabel(),
            out_data.getDigital(tel::DigitalParameter::Id::B8).getLabel());
}

TEST(Aprs, EncodeAndDecodeTelemetryCoefficients) {
  const std::string OUT_FILE_PATH = "aprs_telemetry_coefficients_test.wav";

  namespace tel = signal_easel::aprs::telemetry;

  tel::TelemetryData in_data;
  in_data.setTelemetryStationAddress("TST-1");

  in_data.getAnalog(tel::AnalogParameter::Id::A1)
      .setCoefficients("1", "2", "3");
  in_data.getAnalog(tel::AnalogParameter::Id::A5)
      .setCoefficients("0", "0", "0");

  signal_easel::aprs::TelemetryPacket encode_packet{in_data};
  encode_packet.telemetry_type =
      signal_easel::aprs::Packet::Type::TELEMETRY_COEFFICIENT;
  encode_packet.source_address = "TSTCLL";
  encode_packet.source_ssid = 11;
  encode_packet.destination_address = "APZSEA";
  encode_packet.destination_ssid = 3;

  signal_easel::aprs::Modulator modulator;
  modulator.encode(encode_packet);
  modulator.writeToFile(OUT_FILE_PATH);

  signal_easel::aprs::Demodulator demodulator;
  demodulator.loadAudioFromFile(OUT_FILE_PATH);
  demodulator.processAudioBuffer();

  EXPECT_TRUE(demodulator.lookForAx25Packet());
  EXPECT_EQ(demodulator.getType(),
            signal_easel::aprs::Packet::Type::TELEMETRY_COEFFICIENT);

  // -- Decode --
  signal_easel::aprs::TelemetryPacket decoded_packet;
  EXPECT_TRUE(demodulator.parseTelemetryPacket(decoded_packet));
  EXPECT_EQ(signal_easel::aprs::Packet::Type::TELEMETRY_COEFFICIENT,
            decoded_packet.telemetry_type);

  // Check generics
  EXPECT_EQ(encode_packet.source_address, decoded_packet.source_address);
  EXPECT_EQ(encode_packet.source_ssid, decoded_packet.source_ssid);
  EXPECT_EQ(encode_packet.destination_address,
            decoded_packet.destination_address);
  EXPECT_EQ(encode_packet.destination_ssid, decoded_packet.destination_ssid);

  // Check specifics
  auto &out_data = decoded_packet.telemetry_data;

  EXPECT_EQ(in_data.getTelemetryStationAddress(),
            out_data.getTelemetryStationAddress());

  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A1).getCoefficientA(),
            out_data.getAnalog(tel::AnalogParameter::Id::A1).getCoefficientA());
  EXPECT_EQ(in_data.getAnalog(tel::AnalogParameter::Id::A2).getCoefficientB(),
            out_data.getAnalog(tel::AnalogParameter::Id::A2).getCoefficientB());
}
