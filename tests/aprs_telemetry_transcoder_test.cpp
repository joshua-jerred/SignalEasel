/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file       aprs_telemetry_data__test.cpp
 * @date       2024-08-17
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#include "gtest/gtest.h"

#include <SignalEasel/aprs/telemetry_data.hpp>
#include <SignalEasel/aprs/telemetry_transcoder.hpp>

using namespace signal_easel::aprs::telemetry;

class TelemetryTranscoder_test : public testing::Test {
protected:
  TelemetryTranscoder_test() {}

  const std::string TELEMETRY_STATION_ADDRESS = "TEST-1";

  /// @brief The prefix for telemetry messages (all but data report messages).
  const std::string TELEMETRY_MESSAGE_PREFIX = ":TEST-1   :";

  void SetUp() override {
    data_.setTelemetryStationAddress(TELEMETRY_STATION_ADDRESS);
  }

  TelemetryData data_ = TelemetryData();
};

TEST_F(TelemetryTranscoder_test, encodeDataReportMessage_default) {
  // Sequence number of 0, all analog parameters set to 0, all digital
  // parameters set to 0
  const std::string EXPECTED_PACKET = "T#000,000,000,000,000,000,00000000";

  std::vector<uint8_t> packet;
  EXPECT_TRUE(TelemetryTranscoder::encodeDataReportMessage(data_, packet));

  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, encodeDataReportMessage_setValues) {
  const std::string EXPECTED_PACKET = "T#100,000,010,100,255,099,10000101";

  data_.setSequenceNumber(100);
  data_.getAnalog(AnalogParameter::Id::A1).setRawValue(0);
  data_.getAnalog(AnalogParameter::Id::A2).setRawValue(10);
  data_.getAnalog(AnalogParameter::Id::A3).setRawValue(100);
  data_.getAnalog(AnalogParameter::Id::A4).setRawValue(255);
  data_.getAnalog(AnalogParameter::Id::A5).setRawValue(99);
  data_.getDigital(DigitalParameter::Id::B1).setValue(true);
  data_.getDigital(DigitalParameter::Id::B2).setValue(false);
  data_.getDigital(DigitalParameter::Id::B3).setValue(false);
  data_.getDigital(DigitalParameter::Id::B4).setValue(false);
  data_.getDigital(DigitalParameter::Id::B5).setValue(false);
  data_.getDigital(DigitalParameter::Id::B6).setValue(true);
  data_.getDigital(DigitalParameter::Id::B7).setValue(false);
  data_.getDigital(DigitalParameter::Id::B8).setValue(true);

  std::vector<uint8_t> packet;
  EXPECT_TRUE(TelemetryTranscoder::encodeDataReportMessage(data_, packet));

  std::string packet_string(packet.begin(), packet.end());

  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, encodeDataReportMessage_setComment) {
  // Sequence number of 0, all analog parameters set to 0, all digital
  // parameters set to 0
  const std::string COMMENT = "This is a test comment";
  const std::string EXPECTED_PACKET =
      "T#000,000,000,000,000,000,00000000" + COMMENT;

  data_.setComment(COMMENT);
  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeDataReportMessage(data_, packet);

  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, encodeParameterNameMessage_default) {
  const std::string EXPECTED_PACKET = TELEMETRY_MESSAGE_PREFIX + "PARM.x";

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);

  std::string packet_string(packet.begin(), packet.end());

  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, encodeParameterNameMessage_setAnalogName) {

  data_.getAnalog(AnalogParameter::Id::A1).setName("A1");

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "PARM.A1");

  data_.getAnalog(AnalogParameter::Id::A3).setName("A3");
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "PARM.A1,,A3");

  data_.getAnalog(AnalogParameter::Id::A2).setName("A2");
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "PARM.A1,A2,A3");

  data_.getAnalog(AnalogParameter::Id::A4).setName("A4");
  data_.getAnalog(AnalogParameter::Id::A5).setName("A5");
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "PARM.A1,A2,A3,A4,A5");
}

TEST_F(TelemetryTranscoder_test, encodeParameterNameMessage_setDigitalName) {

  data_.getDigital(DigitalParameter::Id::B1).setName("B1");

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "PARM.x,,,,,B1");

  data_.getDigital(DigitalParameter::Id::B3).setName("B3");
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "PARM.x,,,,,B1,,B3");

  data_.getDigital(DigitalParameter::Id::B2).setName("B2");
  data_.getDigital(DigitalParameter::Id::B4).setName("B4");
  data_.getDigital(DigitalParameter::Id::B5).setName("B5");
  data_.getDigital(DigitalParameter::Id::B6).setName("B6");
  data_.getDigital(DigitalParameter::Id::B7).setName("B7");
  data_.getDigital(DigitalParameter::Id::B8).setName("B8");

  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string,
            TELEMETRY_MESSAGE_PREFIX + "PARM.x,,,,,B1,B2,B3,B4,B5,B6,B7,B8");
}

TEST_F(TelemetryTranscoder_test, encodeParameterNameMessage_setAllNames) {

  data_.getAnalog(AnalogParameter::Id::A1).setName("aaaaaaa");
  data_.getAnalog(AnalogParameter::Id::A2).setName("aaaaaa");
  data_.getAnalog(AnalogParameter::Id::A3).setName("aaaaa");
  data_.getAnalog(AnalogParameter::Id::A4).setName("aaaaa");
  data_.getAnalog(AnalogParameter::Id::A5).setName("aaaa");

  data_.getDigital(DigitalParameter::Id::B1).setName("bbbbb");
  data_.getDigital(DigitalParameter::Id::B2).setName("bbbb");
  data_.getDigital(DigitalParameter::Id::B3).setName("bbb");
  data_.getDigital(DigitalParameter::Id::B4).setName("bbb");
  data_.getDigital(DigitalParameter::Id::B5).setName("bbb");
  data_.getDigital(DigitalParameter::Id::B6).setName("bb");
  data_.getDigital(DigitalParameter::Id::B7).setName("bb");
  data_.getDigital(DigitalParameter::Id::B8).setName("bb");

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX +
                               "PARM.aaaaaaa,aaaaaa,aaaaa,aaaaa,aaaa,"
                               "bbbbb,bbbb,bbb,bbb,bbb,bb,bb,bb");
}

TEST_F(TelemetryTranscoder_test, encodeUnitAndLabelMessage_setAnalogUnit) {

  data_.getAnalog(AnalogParameter::Id::A1).setUnitOrLabel("A1");

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "UNIT.A1");

  data_.getAnalog(AnalogParameter::Id::A3).setUnitOrLabel("A3");
  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "UNIT.A1,,A3");

  data_.getAnalog(AnalogParameter::Id::A2).setUnitOrLabel("A2");
  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "UNIT.A1,A2,A3");

  data_.getAnalog(AnalogParameter::Id::A4).setUnitOrLabel("A4");
  data_.getAnalog(AnalogParameter::Id::A5).setUnitOrLabel("A5");
  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "UNIT.A1,A2,A3,A4,A5");
}

TEST_F(TelemetryTranscoder_test, encodeUnitAndLabelMessage_setDigitalLabel) {

  data_.getDigital(DigitalParameter::Id::B1).setUnitOrLabel("B1");

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "UNIT.x,,,,,B1");

  data_.getDigital(DigitalParameter::Id::B3).setUnitOrLabel("B3");
  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, TELEMETRY_MESSAGE_PREFIX + "UNIT.x,,,,,B1,,B3");

  data_.getDigital(DigitalParameter::Id::B2).setUnitOrLabel("B2");
  data_.getDigital(DigitalParameter::Id::B4).setUnitOrLabel("B4");
  data_.getDigital(DigitalParameter::Id::B5).setUnitOrLabel("B5");
  data_.getDigital(DigitalParameter::Id::B6).setUnitOrLabel("B6");
  data_.getDigital(DigitalParameter::Id::B7).setUnitOrLabel("B7");
  data_.getDigital(DigitalParameter::Id::B8).setUnitOrLabel("B8");

  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string,
            TELEMETRY_MESSAGE_PREFIX + "UNIT.x,,,,,B1,B2,B3,B4,B5,B6,B7,B8");
}

TEST_F(TelemetryTranscoder_test, encodeEquationCoefficientsMessage_default) {
  const std::string EXPECTED_PACKET =
      TELEMETRY_MESSAGE_PREFIX + "EQNS.0,1,0,0,1,0,0,1,0,0,1,0,0,1,0";

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeParameterCoefficientMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test,
       encodeEquationCoefficientsMessage_setCoefficients) {
  const std::string EXPECTED_PACKET =
      TELEMETRY_MESSAGE_PREFIX +
      "EQNS.1,2,3,4.1,4.2,4.3,.01,-.01,0.01,-32,18,.53,0,0,0";

  data_.getAnalog(AnalogParameter::Id::A1).setCoefficients("1", "2", "3");
  data_.getAnalog(AnalogParameter::Id::A2).setCoefficients("4.1", "4.2", "4.3");
  data_.getAnalog(AnalogParameter::Id::A3)
      .setCoefficients(".01", "-.01", "0.01");
  data_.getAnalog(AnalogParameter::Id::A4).setCoefficients("-32", "18", ".53");
  data_.getAnalog(AnalogParameter::Id::A5).setCoefficients("0", "0", "0");

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeParameterCoefficientMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, encodeBitSenseMessage_default) {
  const std::string EXPECTED_PACKET =
      TELEMETRY_MESSAGE_PREFIX + "BITS.11111111";

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeBitSenseMessage(data_, packet);

  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, encodeBitSenseMessage_setBitSense) {
  const std::string EXPECTED_PACKET =
      TELEMETRY_MESSAGE_PREFIX + "BITS.01110101";

  data_.getDigital(DigitalParameter::Id::B1).setBitSense(false);
  data_.getDigital(DigitalParameter::Id::B2).setBitSense(true);
  data_.getDigital(DigitalParameter::Id::B3).setBitSense(true);
  data_.getDigital(DigitalParameter::Id::B4).setBitSense(true);
  data_.getDigital(DigitalParameter::Id::B5).setBitSense(false);
  data_.getDigital(DigitalParameter::Id::B6).setBitSense(true);
  data_.getDigital(DigitalParameter::Id::B7).setBitSense(false);
  data_.getDigital(DigitalParameter::Id::B8).setBitSense(true);

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeBitSenseMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, encodeBitSenseMessage_projectTitle) {
  const std::string PROJECT_TITLE = "Test Project";
  const std::string EXPECTED_PACKET =
      TELEMETRY_MESSAGE_PREFIX + "BITS.11111111," + PROJECT_TITLE;

  data_.setProjectTitle("Test Project");

  std::vector<uint8_t> packet;
  TelemetryTranscoder::encodeBitSenseMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST_F(TelemetryTranscoder_test, decodeMessages_valuesMessage) {
  const std::string COMMENT = "This is a test comment";
  const std::string VALUES_MESSAGE =
      "T#100,000,010,100,255,099,10000101" + COMMENT;

  // Decode the packet
  std::vector<uint8_t> packet(VALUES_MESSAGE.begin(), VALUES_MESSAGE.end());
  EXPECT_TRUE(TelemetryTranscoder::decodeMessage(data_, packet));

  // Verify that the values generated packet is the same as the input packet
  TelemetryTranscoder::encodeDataReportMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, VALUES_MESSAGE);

  // Check the comment
  EXPECT_EQ(data_.getComment(), COMMENT);

  // Check the sequence number
  EXPECT_EQ(data_.getSequenceNumber(), 100);

  // Check the analog values
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A1).getValue(), 0);
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A2).getValue(), 10);
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A3).getValue(), 100);
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A4).getValue(), 255);
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A5).getValue(), 99);

  // Check the digital values
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B1).getValue(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B2).getValue(), false);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B3).getValue(), false);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B4).getValue(), false);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B5).getValue(), false);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B6).getValue(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B7).getValue(), false);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B8).getValue(), true);
}

TEST_F(TelemetryTranscoder_test, decodeMessages_bitSenseMessage) {
  const std::string PROJECT_TITLE = "Test Project";
  const std::string BIT_SENSE_MESSAGE =
      TELEMETRY_MESSAGE_PREFIX + "BITS.10111110," + PROJECT_TITLE;

  // Decode the packet
  std::vector<uint8_t> packet(BIT_SENSE_MESSAGE.begin(),
                              BIT_SENSE_MESSAGE.end());
  EXPECT_TRUE(TelemetryTranscoder::decodeMessage(data_, packet));

  // Verify that the values generated packet is the same as the input packet
  TelemetryTranscoder::encodeBitSenseMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, BIT_SENSE_MESSAGE);

  // Check the project title
  EXPECT_EQ(data_.getProjectTitle(), PROJECT_TITLE);

  // Check the bit sense values
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B1).getBitSense(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B2).getBitSense(), false);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B3).getBitSense(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B4).getBitSense(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B5).getBitSense(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B6).getBitSense(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B7).getBitSense(), true);
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B8).getBitSense(), false);
}

TEST_F(TelemetryTranscoder_test, decodeMessages_parameterNameMessage) {
  const std::string PARAM_NAME_PACKET =
      TELEMETRY_MESSAGE_PREFIX + "PARM.a1,a2,,a4,a5,b1,b2,b3,,b5,b6,b7";

  // Decode the packet
  std::vector<uint8_t> packet(PARAM_NAME_PACKET.begin(),
                              PARAM_NAME_PACKET.end());
  EXPECT_TRUE(TelemetryTranscoder::decodeMessage(data_, packet));

  // Verify that the values generated packet is the same as the input packet
  TelemetryTranscoder::encodeParameterNameMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, PARAM_NAME_PACKET);

  // Validate the analog names
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A1).getName(), "a1");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A2).getName(), "a2");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A3).getName(), "");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A4).getName(), "a4");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A5).getName(), "a5");

  // Validate the digital names
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B1).getName(), "b1");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B2).getName(), "b2");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B3).getName(), "b3");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B4).getName(), "");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B5).getName(), "b5");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B6).getName(), "b6");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B7).getName(), "b7");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B8).getName(), "");
}

TEST_F(TelemetryTranscoder_test, decodeMessages_parameterUnitAndLabelMessage) {
  const std::string PARAM_NAME_PACKET =
      TELEMETRY_MESSAGE_PREFIX + "UNIT.A1,,,A4,A5,,B2,B3,,B5,B6";

  // Decode the packet
  std::vector<uint8_t> packet(PARAM_NAME_PACKET.begin(),
                              PARAM_NAME_PACKET.end());
  EXPECT_TRUE(TelemetryTranscoder::decodeMessage(data_, packet));

  // Verify that the values generated packet is the same as the input packet
  TelemetryTranscoder::encodeUnitAndLabelMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, PARAM_NAME_PACKET);

  // Validate the analog names
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A1).getLabel(), "A1");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A2).getLabel(), "");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A3).getLabel(), "");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A4).getLabel(), "A4");
  EXPECT_EQ(data_.getAnalog(AnalogParameter::Id::A5).getLabel(), "A5");

  // Validate the digital labels
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B1).getLabel(), "");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B2).getLabel(), "B2");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B3).getLabel(), "B3");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B4).getLabel(), "");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B5).getLabel(), "B5");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B6).getLabel(), "B6");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B7).getLabel(), "");
  EXPECT_EQ(data_.getDigital(DigitalParameter::Id::B8).getLabel(), "");
}

TEST_F(TelemetryTranscoder_test, decodeMessages_parameterCoefficientsMessage) {
  const std::string PARAM_COEF_MESSAGE = TELEMETRY_MESSAGE_PREFIX +
                                         "EQNS.1,2,3,4.1,4.2,4.3,.01,-.01,0.01,"
                                         "-32,18,.53,0,0,0";

  // Decode the packet
  std::vector<uint8_t> packet(PARAM_COEF_MESSAGE.begin(),
                              PARAM_COEF_MESSAGE.end());
  EXPECT_TRUE(TelemetryTranscoder::decodeMessage(data_, packet));

  // Verify that the values generated packet is the same as the input packet
  TelemetryTranscoder::encodeParameterCoefficientMessage(data_, packet);
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, PARAM_COEF_MESSAGE);

  // Validate the analog names
  std::string coef_a;
  std::string coef_b;
  std::string coef_c;
  data_.getAnalog(AnalogParameter::Id::A1)
      .getCoefficients(coef_a, coef_b, coef_c);
  EXPECT_EQ(coef_a, "1");
  EXPECT_EQ(coef_b, "2");
  EXPECT_EQ(coef_c, "3");

  data_.getAnalog(AnalogParameter::Id::A2)
      .getCoefficients(coef_a, coef_b, coef_c);
  EXPECT_EQ(coef_a, "4.1");
  EXPECT_EQ(coef_b, "4.2");
  EXPECT_EQ(coef_c, "4.3");

  data_.getAnalog(AnalogParameter::Id::A3)
      .getCoefficients(coef_a, coef_b, coef_c);
  EXPECT_EQ(coef_a, ".01");
  EXPECT_EQ(coef_b, "-.01");
  EXPECT_EQ(coef_c, "0.01");

  data_.getAnalog(AnalogParameter::Id::A4)
      .getCoefficients(coef_a, coef_b, coef_c);
  EXPECT_EQ(coef_a, "-32");
  EXPECT_EQ(coef_b, "18");
  EXPECT_EQ(coef_c, ".53");

  data_.getAnalog(AnalogParameter::Id::A5)
      .getCoefficients(coef_a, coef_b, coef_c);
  EXPECT_EQ(coef_a, "0");
  EXPECT_EQ(coef_b, "0");
  EXPECT_EQ(coef_c, "0");
}