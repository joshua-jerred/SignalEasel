/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file       aprs_telemetry_data_test.cpp
 * @date       2024-08-17
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#include "gtest/gtest.h"

#include <SignalEasel/aprs/telemetry_data.hpp>

using namespace signal_easel::aprs::telemetry;

TEST(aprs_telemetry_data, getAnalogParamater) {
  TelemetryData data;

  std::vector<Parameter::Id> analog_ids = {Parameter::Id::A1, Parameter::Id::A2,
                                           Parameter::Id::A3, Parameter::Id::A4,
                                           Parameter::Id::A5};

  for (auto id : analog_ids) {
    auto &parameter = data.getAnalog(id);
    EXPECT_EQ(parameter.getId(), id);
    EXPECT_EQ(parameter.getType(), Parameter::Type::Analog);
  }
}

TEST(aprs_telemetry_data, getDigitalParameter) {
  TelemetryData data;

  std::vector<Parameter::Id> digital_ids = {
      Parameter::Id::B1, Parameter::Id::B2, Parameter::Id::B3,
      Parameter::Id::B4, Parameter::Id::B5, Parameter::Id::B6,
      Parameter::Id::B7, Parameter::Id::B8};

  for (auto id : digital_ids) {
    auto &parameter = data.getDigital(id);
    EXPECT_EQ(parameter.getId(), id);
    EXPECT_EQ(parameter.getType(), Parameter::Type::Digital);
  }
}

TEST(aprs_telemetry_data, setSequenceNumber) {
  TelemetryData data;

  EXPECT_TRUE(data.setSequenceNumber(0));
  EXPECT_EQ(data.getSequenceNumber(), 0);
  EXPECT_STREQ(data.getSequenceNumberString().c_str(), "000");

  EXPECT_TRUE(data.setSequenceNumber(1));
  EXPECT_EQ(data.getSequenceNumber(), 1);
  EXPECT_STREQ(data.getSequenceNumberString().c_str(), "001");

  EXPECT_FALSE(data.setSequenceNumber(1000));
  EXPECT_EQ(data.getSequenceNumber(), 0);
  EXPECT_STREQ(data.getSequenceNumberString().c_str(), "000");

  EXPECT_TRUE(data.setSequenceNumber(999));
  EXPECT_EQ(data.getSequenceNumber(), 999);
  EXPECT_STREQ(data.getSequenceNumberString().c_str(), "999");

  EXPECT_TRUE(data.setSequenceNumber(123));
  EXPECT_EQ(data.getSequenceNumber(), 123);
  EXPECT_STREQ(data.getSequenceNumberString().c_str(), "123");
}

TEST(aprs_telemetry_data, setComment) {
  TelemetryData data;

  EXPECT_TRUE(data.setComment("Hello World!"));
  EXPECT_STREQ(data.getComment().c_str(), "Hello World!");

  const std::string long_comment =
      "This is a very long comment. Some might even say it's too long. "
      "Comments get truncated after 220 characters. The set comment function "
      "will also return false if that's the case! This comment is currently "
      "223 characters long.";

  EXPECT_FALSE(data.setComment(long_comment));
  EXPECT_STREQ(data.getComment().c_str(), long_comment.substr(0, 220).c_str());
}

TEST(aprs_telemetry_data, setProjectTitle) {
  TelemetryData data;

  EXPECT_TRUE(data.setProjectTitle("Hello World!"));
  EXPECT_STREQ(data.getProjectTitle().c_str(), "Hello World!");

  const std::string long_project_title = "There is a 22 character limit.";
  EXPECT_FALSE(data.setProjectTitle(long_project_title));
  EXPECT_STREQ(data.getProjectTitle().c_str(), "There is a 22 characte");
}
