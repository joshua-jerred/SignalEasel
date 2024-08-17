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

#include <SignalEasel/aprs/telemetry_parameter.hpp>
#include <iostream> /// @todo remove

using namespace signal_easel::aprs::telemetry;

TEST(telemetry_parameters_test, AnalogParameter_setName) {
  AnalogParameter parameter_a1(Parameter::Id::A1);
  AnalogParameter parameter_a2(Parameter::Id::A2);

  EXPECT_EQ(parameter_a1.getType(), Parameter::Type::Analog);

  // A1 is special. It must be non-empty.
  EXPECT_FALSE(parameter_a1.setName(""));
  EXPECT_TRUE(parameter_a1.setName("a"));
  EXPECT_EQ(parameter_a1.getName(), "a");

  // A2 is less special. It can be empty.
  EXPECT_TRUE(parameter_a2.setName(""));
  EXPECT_EQ(parameter_a2.getName(), "");
}

TEST(telemetry_parameters_test, DigitalParameter_setName) {
  DigitalParameter parameter_b8(Parameter::Id::B8);
  EXPECT_EQ(parameter_b8.getType(), Parameter::Type::Digital);

  // B8 can be empty or up to 2 characters.
  EXPECT_TRUE(parameter_b8.setName(""));
  EXPECT_EQ(parameter_b8.getName(), "");
  EXPECT_TRUE(parameter_b8.setName("a"));
  EXPECT_EQ(parameter_b8.getName(), "a");
  EXPECT_TRUE(parameter_b8.setName("ab"));
  EXPECT_EQ(parameter_b8.getName(), "ab");
  EXPECT_FALSE(parameter_b8.setName("abc"));
  EXPECT_EQ(parameter_b8.getName(), "ab");
}

TEST(telemetry_parameters_test, AnalogParameter_getCoefficient_default) {
  AnalogParameter parameter(Parameter::Id::A1);

  EXPECT_EQ(parameter.getCoefficientA(), "0");
  EXPECT_EQ(parameter.getCoefficientB(), "1");
  EXPECT_EQ(parameter.getCoefficientC(), "0");
}

TEST(telemetry_parameters_test, AnalogParameter_setCoefficient) {
  // We're not too picky. Just make sure it's a valid number.
  const std::vector<std::string> VALID_COEFFICIENTS = {
      ".0", "0",    "0.",       "0.0", "-0", "-.0", "-0.0",      "-0.",
      "1",  "123.", "1234.568", "120", "00", "123", "999999999", "0.075"};

  const std::vector<std::string> INVALID_COEFFICIENTS = {"-", ".", "0.0.0",
                                                         "-.", ""};

  AnalogParameter parameter(Parameter::Id::A1);

  for (const auto &valid_coefficient : VALID_COEFFICIENTS) {
    EXPECT_TRUE(parameter.setCoefficientA(valid_coefficient))
        << "Failed to set coefficient: " << valid_coefficient;

    if (valid_coefficient.empty()) {
      EXPECT_EQ(parameter.getCoefficientA(), "0");
    } else {
      EXPECT_EQ(parameter.getCoefficientA(), valid_coefficient);
    }
  }

  for (const auto &invalid_coefficient : INVALID_COEFFICIENTS) {
    EXPECT_FALSE(parameter.setCoefficientA(invalid_coefficient))
        << "Failed to throw on invalid coefficient: " << invalid_coefficient;
    EXPECT_NE(parameter.getCoefficientA(), invalid_coefficient);
  }
}

TEST(telemetry_parameters_test, AnalogParameter_getRawValue) {
  AnalogParameter parameter(Parameter::Id::A1);

  EXPECT_EQ(parameter.getRawValue(), 0) << "Default raw value should be 0.";

  constexpr uint8_t TEST_VALUE = 250;
  parameter.setRawValue(TEST_VALUE);
  EXPECT_EQ(parameter.getCalculatedValue(), TEST_VALUE);
}

TEST(telemetry_parameters_test, AnalogParameter_getScaledValue_default) {
  AnalogParameter parameter(Parameter::Id::A1);

  constexpr uint8_t TEST_RAW_VALUE = 250;
  parameter.setRawValue(TEST_RAW_VALUE);

  // Default coefficients are 0, 1, 0, so the scaled value should be the raw
  // value.
  EXPECT_NEAR(parameter.getCalculatedValue(), TEST_RAW_VALUE, 0.00001);
}

TEST(telemetry_parameters_test, AnalogParameter_getCalculatedValue) {
  struct Test {
    uint8_t raw_value;
    std::string coefficient_a;
    std::string coefficient_b;
    std::string coefficient_c;
    double expected_value;
  };

  const std::vector<Test> TEST_LIST = {
      Test{0, "0", "1", "0", 0.0},  Test{0, "0", "0", "0", 0.0},
      Test{0, "0", "0", "1", 1.0},  Test{5, "0", "0", "1", 1.0},
      Test{5, "0", "1", "0", 5.0},  Test{5, "0", "1", "1", 6.0},
      Test{5, "1", "0", "0", 25.0}, Test{197, "0", "0.075", "0", 14.775}};

  AnalogParameter parameter(Parameter::Id::A1);

  for (const auto &test : TEST_LIST) {
    parameter.setRawValue(test.raw_value);
    parameter.setCoefficients(test.coefficient_a, test.coefficient_b,
                              test.coefficient_c);

    EXPECT_NEAR(parameter.getCalculatedValue(), test.expected_value, 0.00001);
  }
}