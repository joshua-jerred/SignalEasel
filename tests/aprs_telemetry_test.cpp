/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   aprs_telemetry_test.cpp
 * @date   2024-08-16
 * @brief  APRS implementation
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#include "gtest/gtest.h"

#include <SignalEasel/aprs_telemetry.hpp>
#include <iostream> /// @todo remove

using namespace signal_easel::aprs;

TEST(aprs_telemetry_test, AnalogParameter_setName) {
  AnalogParameter parameter;

  // Require for an ID to be set before setting a name
  EXPECT_THROW(parameter.setName("Test"), signal_easel::Exception);
  EXPECT_TRUE(parameter.getName().empty());

  // Set the ID and then set the name
  parameter.setId(AnalogParameter::Id::A1);
  EXPECT_NO_THROW(parameter.setName("Test"));
  EXPECT_EQ(parameter.getName(), "Test");
}

TEST(aprs_telemetry_test, AnalogParameter_setCoefficient) {
  // We're not too picky. Just make sure it's a valid number.
  const std::vector<std::string> VALID_COEFFICIENTS = {
      ".0", "0",    "0.",       "0.0", "-0", "-.0", "-0.0",     "-0.",
      "1",  "123.", "1234.568", "120", "00", "123", "999999999"};

  const std::vector<std::string> INVALID_COEFFICIENTS = {"-", ".", "0.0.0",
                                                         "-.", ""};

  AnalogParameter parameter;

  for (const auto &valid_coefficient : VALID_COEFFICIENTS) {
    EXPECT_NO_THROW(parameter.setACoefficient(valid_coefficient))
        << "Failed to set coefficient: " << valid_coefficient;

    if (valid_coefficient.empty()) {
      EXPECT_EQ(parameter.getACoefficient(), "0");
    } else {
      EXPECT_EQ(parameter.getACoefficient(), valid_coefficient);
    }
  }

  for (const auto &invalid_coefficient : INVALID_COEFFICIENTS) {
    EXPECT_THROW(parameter.setACoefficient(invalid_coefficient),
                 signal_easel::Exception)
        << "Failed to throw on invalid coefficient: " << invalid_coefficient;
  }
}

TEST(TelemetryHandler_test, encodeDataReportMessage_default) {
  // Sequence number of 0, all analog parameters set to 0, all digital
  // parameters set to 0
  const std::string EXPECTED_PACKET = "T#000,000,000,000,000,000,00000000";

  TelemetryHandler handler;
  std::vector<uint8_t> packet = handler.encodeDataReportMessage();

  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeDataReportMessage_setValues) {
  const std::string EXPECTED_PACKET = "T#100,000,010,100,255,099,10000101";

  TelemetryHandler handler;

  handler.setSequenceNumber(100);
  handler.setAnalogValue(AnalogParameter::Id::A1, 0);
  handler.setAnalogValue(AnalogParameter::Id::A2, 10);
  handler.setAnalogValue(AnalogParameter::Id::A3, 100);
  handler.setAnalogValue(AnalogParameter::Id::A4, 255);
  handler.setAnalogValue(AnalogParameter::Id::A5, 99);

  handler.setDigitalValue(DigitalParameter::Id::B1, true);
  handler.setDigitalValue(DigitalParameter::Id::B2, false);
  handler.setDigitalValue(DigitalParameter::Id::B3, false);
  handler.setDigitalValue(DigitalParameter::Id::B4, false);
  handler.setDigitalValue(DigitalParameter::Id::B5, false);
  handler.setDigitalValue(DigitalParameter::Id::B6, true);
  handler.setDigitalValue(DigitalParameter::Id::B7, false);
  handler.setDigitalValue(DigitalParameter::Id::B8, true);

  std::vector<uint8_t> packet = handler.encodeDataReportMessage();

  std::string packet_string(packet.begin(), packet.end());

  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeDataReportMessage_setComment) {
  // Sequence number of 0, all analog parameters set to 0, all digital
  // parameters set to 0
  const std::string COMMENT = "This is a test comment";
  const std::string EXPECTED_PACKET =
      "T#000,000,000,000,000,000,00000000" + COMMENT;

  TelemetryHandler handler;
  handler.setComment(COMMENT);
  std::vector<uint8_t> packet = handler.encodeDataReportMessage();

  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeParameterNameMessage_default) {
  const std::string EXPECTED_PACKET = "PARM.x";

  TelemetryHandler handler;
  std::vector<uint8_t> packet = handler.encodeParameterNameMessage();

  std::string packet_string(packet.begin(), packet.end());

  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeParameterNameMessage_setAnalogName) {
  TelemetryHandler handler;

  handler.setAnalogName(AnalogParameter::Id::A1, "A1");

  std::vector<uint8_t> packet = handler.encodeParameterNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "PARM.A1");

  handler.setAnalogName(AnalogParameter::Id::A3, "A3");
  packet = handler.encodeParameterNameMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "PARM.A1,,A3");

  handler.setAnalogName(AnalogParameter::Id::A2, "A2");
  packet = handler.encodeParameterNameMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "PARM.A1,A2,A3");

  handler.setAnalogName(AnalogParameter::Id::A4, "A4");
  handler.setAnalogName(AnalogParameter::Id::A5, "A5");
  packet = handler.encodeParameterNameMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "PARM.A1,A2,A3,A4,A5");
}

TEST(TelemetryHandler_test, encodeParameterNameMessage_setDigitalName) {
  TelemetryHandler handler;

  handler.setDigitalName(DigitalParameter::Id::B1, "B1");

  std::vector<uint8_t> packet = handler.encodeParameterNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "PARM.x,,,,,B1");

  handler.setDigitalName(DigitalParameter::Id::B3, "B3");
  packet = handler.encodeParameterNameMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "PARM.x,,,,,B1,,B3");

  handler.setDigitalName(DigitalParameter::Id::B2, "B2");
  handler.setDigitalName(DigitalParameter::Id::B4, "B4");
  handler.setDigitalName(DigitalParameter::Id::B5, "B5");
  handler.setDigitalName(DigitalParameter::Id::B6, "B6");
  handler.setDigitalName(DigitalParameter::Id::B7, "B7");
  handler.setDigitalName(DigitalParameter::Id::B8, "B8");

  packet = handler.encodeParameterNameMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "PARM.x,,,,,B1,B2,B3,B4,B5,B6,B7,B8");
}

TEST(TelemetryHandler_test, encodeParameterNameMessage_setAllNames) {
  TelemetryHandler handler;

  handler.setAnalogName(AnalogParameter::Id::A1, "aaaaaaa");
  handler.setAnalogName(AnalogParameter::Id::A2, "aaaaaa");
  handler.setAnalogName(AnalogParameter::Id::A3, "aaaaa");
  handler.setAnalogName(AnalogParameter::Id::A4, "aaaaa");
  handler.setAnalogName(AnalogParameter::Id::A5, "aaaa");

  handler.setDigitalName(DigitalParameter::Id::B1, "bbbbb");
  handler.setDigitalName(DigitalParameter::Id::B2, "bbbb");
  handler.setDigitalName(DigitalParameter::Id::B3, "bbb");
  handler.setDigitalName(DigitalParameter::Id::B4, "bbb");
  handler.setDigitalName(DigitalParameter::Id::B5, "bbb");
  handler.setDigitalName(DigitalParameter::Id::B6, "bb");
  handler.setDigitalName(DigitalParameter::Id::B7, "bb");
  handler.setDigitalName(DigitalParameter::Id::B8, "bb");

  std::vector<uint8_t> packet = handler.encodeParameterNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(
      packet_string,
      "PARM.aaaaaaa,aaaaaa,aaaaa,aaaaa,aaaa,bbbbb,bbbb,bbb,bbb,bbb,bb,bb,bb");
}

TEST(TelemetryHandler_test, encodeUntilAndLabelMessage_setAnalogUnit) {
  TelemetryHandler handler;

  handler.setAnalogUnit(AnalogParameter::Id::A1, "A1");

  std::vector<uint8_t> packet = handler.encodeUntilAndLabelMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "UNIT.A1");

  handler.setAnalogUnit(AnalogParameter::Id::A3, "A3");
  packet = handler.encodeUntilAndLabelMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "UNIT.A1,,A3");

  handler.setAnalogUnit(AnalogParameter::Id::A2, "A2");
  packet = handler.encodeUntilAndLabelMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "UNIT.A1,A2,A3");

  handler.setAnalogUnit(AnalogParameter::Id::A4, "A4");
  handler.setAnalogUnit(AnalogParameter::Id::A5, "A5");
  packet = handler.encodeUntilAndLabelMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "UNIT.A1,A2,A3,A4,A5");
}

TEST(TelemetryHandler_test, encodeUntilAndLabelMessage_setDigitalLabel) {
  TelemetryHandler handler;

  handler.setDigitalLabel(DigitalParameter::Id::B1, "B1");

  std::vector<uint8_t> packet = handler.encodeUntilAndLabelMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "UNIT.x,,,,,B1");

  handler.setDigitalLabel(DigitalParameter::Id::B3, "B3");
  packet = handler.encodeUntilAndLabelMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "UNIT.x,,,,,B1,,B3");

  handler.setDigitalLabel(DigitalParameter::Id::B2, "B2");
  handler.setDigitalLabel(DigitalParameter::Id::B4, "B4");
  handler.setDigitalLabel(DigitalParameter::Id::B5, "B5");
  handler.setDigitalLabel(DigitalParameter::Id::B6, "B6");
  handler.setDigitalLabel(DigitalParameter::Id::B7, "B7");
  handler.setDigitalLabel(DigitalParameter::Id::B8, "B8");

  packet = handler.encodeUntilAndLabelMessage();
  packet_string = std::string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, "UNIT.x,,,,,B1,B2,B3,B4,B5,B6,B7,B8");
}

TEST(TelemetryHandler_test, encodeEquationCoefficientsMessage_default) {
  const std::string EXPECTED_PACKET = "EQNS.0,1,0,0,1,0,0,1,0,0,1,0,0,1,0";

  TelemetryHandler handler;

  std::vector<uint8_t> packet = handler.encodeEquationCoefficientsMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeEquationCoefficientsMessage_setCoefficients) {
  const std::string EXPECTED_PACKET =
      "EQNS.1,2,3,4.1,4.2,4.3,.01,-.01,0.01,-32,18,.53,0,0,0";

  TelemetryHandler handler;
  handler.setAnalogCoefficients(AnalogParameter::Id::A1, "1", "2", "3");
  handler.setAnalogCoefficients(AnalogParameter::Id::A2, "4.1", "4.2", "4.3");
  handler.setAnalogCoefficients(AnalogParameter::Id::A3, ".01", "-.01", "0.01");
  handler.setAnalogCoefficients(AnalogParameter::Id::A4, "-32", "18", ".53");
  handler.setAnalogCoefficients(AnalogParameter::Id::A5, "0", "0", "0");

  std::vector<uint8_t> packet = handler.encodeEquationCoefficientsMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeBitSenseProjectNameMessage_default) {
  const std::string EXPECTED_PACKET = "BITS.11111111";

  TelemetryHandler handler;

  std::vector<uint8_t> packet = handler.encodeBitSenseProjectNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeBitSenseProjectNameMessage_setBitSense) {
  const std::string EXPECTED_PACKET = "BITS.01110101";

  TelemetryHandler handler;
  handler.setBitSense(DigitalParameter::Id::B1, false);
  handler.setBitSense(DigitalParameter::Id::B2, true);
  handler.setBitSense(DigitalParameter::Id::B3, true);
  handler.setBitSense(DigitalParameter::Id::B4, true);
  handler.setBitSense(DigitalParameter::Id::B5, false);
  handler.setBitSense(DigitalParameter::Id::B6, true);
  handler.setBitSense(DigitalParameter::Id::B7, false);
  handler.setBitSense(DigitalParameter::Id::B8, true);

  std::vector<uint8_t> packet = handler.encodeBitSenseProjectNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, encodeBitSenseProjectNameMessage_projectTitle) {
  const std::string PROJECT_TITLE = "Test Project";
  const std::string EXPECTED_PACKET = "BITS.11111111," + PROJECT_TITLE;

  TelemetryHandler handler;
  handler.setProjectTitle("Test Project");

  std::vector<uint8_t> packet = handler.encodeBitSenseProjectNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, EXPECTED_PACKET);
}

TEST(TelemetryHandler_test, decodeMessages_valuesMessage) {
  const std::string COMMENT = "This is a test comment";
  const std::string VALUES_MESSAGE =
      "T#100,000,010,100,255,099,10000101" + COMMENT;

  TelemetryHandler handler;

  // Decode the packet
  std::vector<uint8_t> packet(VALUES_MESSAGE.begin(), VALUES_MESSAGE.end());
  EXPECT_TRUE(handler.decodeMessage(packet));

  // Verify that the values generated packet is the same as the input packet
  packet = handler.encodeDataReportMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, VALUES_MESSAGE);

  // Check the comment
  EXPECT_EQ(handler.getComment(), COMMENT);

  // Check the sequence number
  EXPECT_EQ(handler.getSequenceNumber(), 100);

  // Check the analog values
  EXPECT_EQ(handler.getAnalogValue(AnalogParameter::Id::A1), 0);
  EXPECT_EQ(handler.getAnalogValue(AnalogParameter::Id::A2), 10);
  EXPECT_EQ(handler.getAnalogValue(AnalogParameter::Id::A3), 100);
  EXPECT_EQ(handler.getAnalogValue(AnalogParameter::Id::A4), 255);
  EXPECT_EQ(handler.getAnalogValue(AnalogParameter::Id::A5), 99);

  // Check the digital values
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B1), true);
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B2), false);
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B3), false);
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B4), false);
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B5), false);
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B6), true);
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B7), false);
  EXPECT_EQ(handler.getDigitalValue(DigitalParameter::Id::B8), true);
}

TEST(TelemetryHandler_test, decodeMessages_bitSenseMessage) {
  const std::string PROJECT_TITLE = "Test Project";
  const std::string BIT_SENSE_MESSAGE = "BITS.10111110," + PROJECT_TITLE;

  TelemetryHandler handler;

  // Decode the packet
  std::vector<uint8_t> packet(BIT_SENSE_MESSAGE.begin(),
                              BIT_SENSE_MESSAGE.end());
  EXPECT_TRUE(handler.decodeMessage(packet));

  // Verify that the values generated packet is the same as the input packet
  packet = handler.encodeBitSenseProjectNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, BIT_SENSE_MESSAGE);

  // Check the project title
  EXPECT_EQ(handler.getProjectTitle(), PROJECT_TITLE);

  // Check the bit sense values
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B1), true);
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B2), false);
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B3), true);
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B4), true);
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B5), true);
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B6), true);
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B7), true);
  EXPECT_EQ(handler.getBitSense(DigitalParameter::Id::B8), false);
}

TEST(TelemetryHandler_test, decodeMessages_parameterNameMessage) {
  const std::string PARAM_NAME_PACKET = "PARM.a1,a2,,a4,a5,b1,b2,b3,,b5,b6,b7";

  TelemetryHandler handler;

  // Decode the packet
  std::vector<uint8_t> packet(PARAM_NAME_PACKET.begin(),
                              PARAM_NAME_PACKET.end());
  EXPECT_TRUE(handler.decodeMessage(packet));

  // Verify that the values generated packet is the same as the input packet
  packet = handler.encodeParameterNameMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, PARAM_NAME_PACKET);

  // Validate the analog names
  EXPECT_EQ(handler.getAnalogName(AnalogParameter::Id::A1), "a1");
  EXPECT_EQ(handler.getAnalogName(AnalogParameter::Id::A2), "a2");
  EXPECT_EQ(handler.getAnalogName(AnalogParameter::Id::A3), "");
  EXPECT_EQ(handler.getAnalogName(AnalogParameter::Id::A4), "a4");
  EXPECT_EQ(handler.getAnalogName(AnalogParameter::Id::A5), "a5");

  // Validate the digital names
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B1), "b1");
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B2), "b2");
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B3), "b3");
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B4), "");
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B5), "b5");
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B6), "b6");
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B7), "b7");
  EXPECT_EQ(handler.getDigitalName(DigitalParameter::Id::B8), "");
}

TEST(TelemetryHandler_test, decodeMessages_parameterUnitAndLabelMessage) {
  const std::string PARAM_NAME_PACKET = "UNIT.A1,,,A4,A5,,B2,B3,,B5,B6";

  TelemetryHandler handler;

  // Decode the packet
  std::vector<uint8_t> packet(PARAM_NAME_PACKET.begin(),
                              PARAM_NAME_PACKET.end());
  EXPECT_TRUE(handler.decodeMessage(packet));

  // Verify that the values generated packet is the same as the input packet
  packet = handler.encodeUntilAndLabelMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, PARAM_NAME_PACKET);

  // Validate the analog names
  EXPECT_EQ(handler.getAnalogUnit(AnalogParameter::Id::A1), "A1");
  EXPECT_EQ(handler.getAnalogUnit(AnalogParameter::Id::A2), "");
  EXPECT_EQ(handler.getAnalogUnit(AnalogParameter::Id::A3), "");
  EXPECT_EQ(handler.getAnalogUnit(AnalogParameter::Id::A4), "A4");
  EXPECT_EQ(handler.getAnalogUnit(AnalogParameter::Id::A5), "A5");

  // Validate the digital labels
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B1), "");
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B2), "B2");
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B3), "B3");
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B4), "");
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B5), "B5");
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B6), "B6");
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B7), "");
  EXPECT_EQ(handler.getDigitalLabel(DigitalParameter::Id::B8), "");
}

TEST(TelemetryHandler_test, decodeMessages_parameterCoefficientsMessage) {
  const std::string PARAM_COEF_MESSAGE = "EQNS.1,2,3,4.1,4.2,4.3,.01,-.01,0.01,"
                                         "-32,18,.53,0,0,0";

  TelemetryHandler handler;

  // Decode the packet
  std::vector<uint8_t> packet(PARAM_COEF_MESSAGE.begin(),
                              PARAM_COEF_MESSAGE.end());
  EXPECT_TRUE(handler.decodeMessage(packet));

  // Verify that the values generated packet is the same as the input packet
  packet = handler.encodeEquationCoefficientsMessage();
  std::string packet_string(packet.begin(), packet.end());
  EXPECT_EQ(packet_string, PARAM_COEF_MESSAGE);

  // Validate the analog names
  std::string coef_a, coef_b, coef_c;
  handler.getAnalogCoefficients(AnalogParameter::Id::A1, coef_a, coef_b,
                                coef_c);
  EXPECT_EQ(coef_a, "1");
  EXPECT_EQ(coef_b, "2");
  EXPECT_EQ(coef_c, "3");

  handler.getAnalogCoefficients(AnalogParameter::Id::A2, coef_a, coef_b,
                                coef_c);
  EXPECT_EQ(coef_a, "4.1");
  EXPECT_EQ(coef_b, "4.2");
  EXPECT_EQ(coef_c, "4.3");

  handler.getAnalogCoefficients(AnalogParameter::Id::A3, coef_a, coef_b,
                                coef_c);
  EXPECT_EQ(coef_a, ".01");
  EXPECT_EQ(coef_b, "-.01");
  EXPECT_EQ(coef_c, "0.01");

  handler.getAnalogCoefficients(AnalogParameter::Id::A4, coef_a, coef_b,
                                coef_c);
  EXPECT_EQ(coef_a, "-32");
  EXPECT_EQ(coef_b, "18");
  EXPECT_EQ(coef_c, ".53");

  handler.getAnalogCoefficients(AnalogParameter::Id::A5, coef_a, coef_b,
                                coef_c);
  EXPECT_EQ(coef_a, "0");
  EXPECT_EQ(coef_b, "0");
  EXPECT_EQ(coef_c, "0");
}