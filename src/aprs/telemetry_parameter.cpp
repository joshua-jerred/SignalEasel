/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file   telemetry_parameter.cpp
 * @date       2024-08-17
 * @copyright  2024
 * @license    {license}
 */

#include <regex>

#include <SignalEasel/aprs/telemetry_parameter.hpp>

namespace signal_easel::aprs::telemetry {

bool validateCoefficient(const std::string &coefficient,
                         double &numerical_value) {
  // Checks not done by regex first
  if (coefficient.size() > 9 || coefficient.empty()) { // Max length of 9
    return false;
  }

  if (coefficient.size() == 1 && (coefficient == "." || coefficient == "-")) {
    return false;
  }

  if (coefficient.size() == 2 && coefficient == "-.") {
    return false;
  }

  // Regex check
  const std::regex k_coefficient_regex("^-?\\d*\\.?\\d*$");
  if (!std ::regex_match(coefficient, k_coefficient_regex)) {
    return false;
  }

  // Final check of conversion
  try {
    numerical_value = std::stod(coefficient);
  } catch (const std::invalid_argument &e) {
    return false;
  }

  return true;
};

bool validateCoefficient(const std::string &coefficient) {
  double numerical_value;
  return validateCoefficient(coefficient, numerical_value);
}

Parameter::Parameter(Id id, Type type) : id_(id), type_{type} {
  if (id_ == Id::A1) { // Spec requires A1 to be non-empty.
    name_ = "x";
    unit_or_label_ = "x";
  }
}

bool Parameter::setName(const std::string &name) {
  if (!validateParameterDescriptor(name)) {
    return false;
  }
  name_ = name;
  return true;
}

bool Parameter::setUnitOrLabel(const std::string &unit_or_label) {
  if (!validateParameterDescriptor(unit_or_label)) {
    return false;
  }
  unit_or_label_ = unit_or_label;
  return true;
}

bool Parameter::validateParameterDescriptor(const std::string &descriptor) {
  // In the spec, A1 *does not include a comma in the length*, but must be
  // of at least length 1. All others may have a length of 1.
  constexpr size_t MIN_NAME_LENGTH_A1 = 1;
  constexpr size_t MAX_NAME_LENGTH_A1 = 7;
  constexpr size_t MAX_NAME_LENGTH_A2 = 6;
  constexpr size_t MAX_NAME_LENGTH_A3_A4 = 5;
  constexpr size_t MAX_NAME_LENGTH_A5 = 4;
  constexpr size_t MAX_NAME_LENGTH_B1 = 5;
  constexpr size_t MAX_NAME_LENGTH_B2 = 4;
  constexpr size_t MAX_NAME_LENGTH_B3_B4_B5 = 3;
  constexpr size_t MAX_NAME_LENGTH_B6_B7_B8 = 2;

  const size_t k_descriptor_size = descriptor.size();

  switch (id_) {
  case Id::A1:
    return k_descriptor_size >= MIN_NAME_LENGTH_A1 &&
           k_descriptor_size <= MAX_NAME_LENGTH_A1;
  case Id::A2:
    return k_descriptor_size <= MAX_NAME_LENGTH_A2;
  case Id::A3:
  case Id::A4:
    return k_descriptor_size <= MAX_NAME_LENGTH_A3_A4;
  case Id::A5:
    return k_descriptor_size <= MAX_NAME_LENGTH_A5;
    break;
  case Id::B1:
    return k_descriptor_size <= MAX_NAME_LENGTH_B1;
  case Id::B2:
    return k_descriptor_size <= MAX_NAME_LENGTH_B2;
  case Id::B3:
  case Id::B4:
  case Id::B5:
    return k_descriptor_size <= MAX_NAME_LENGTH_B3_B4_B5;
  case Id::B6:
  case Id::B7:
  case Id::B8:
    return k_descriptor_size <= MAX_NAME_LENGTH_B6_B7_B8;
  default:
    return false;
  };
}

AnalogParameter::AnalogParameter(Id id) : Parameter(id, Type::Analog) {
  if (Parameter::getType(id) != Type::Analog) {
    throw Exception(Exception::Id::APRS_TELEMETRY,
                    "AnalogParameter must be analog.");
  }
}

double AnalogParameter::getCalculatedValue() const {
  // a * x^2 + b * x + c
  return (a_ * static_cast<double>(raw_value_) *
          static_cast<double>(raw_value_)) +
         (b_ * static_cast<double>(raw_value_)) + c_;
}

bool AnalogParameter::setCoefficientA(const std::string &coefficient) {
  if (!validateCoefficient(coefficient, a_)) {
    return false;
  }
  coefficient_a_ = coefficient;
  return true;
}

bool AnalogParameter::setCoefficientB(const std::string &coefficient) {
  if (!validateCoefficient(coefficient, b_)) {
    return false;
  }
  coefficient_b_ = coefficient;
  return true;
}

bool AnalogParameter::setCoefficientC(const std::string &coefficient) {
  if (!validateCoefficient(coefficient, c_)) {
    return false;
  }
  coefficient_c_ = coefficient;
  return true;
}

double AnalogParameter::getCoefficientAd() const { return a_; }

double AnalogParameter::getCoefficientBd() const { return b_; }

double AnalogParameter::getCoefficientCd() const { return c_; }

bool AnalogParameter::setCoefficients(const std::string &coefficient_a,
                                      const std::string &coefficient_b,
                                      const std::string &coefficient_c) {
  return setCoefficientA(coefficient_a) && setCoefficientB(coefficient_b) &&
         setCoefficientC(coefficient_c);
}

DigitalParameter::DigitalParameter(Id id) : Parameter(id, Type::Digital) {
  if (Parameter::getType(id) != Type::Digital) {
    throw Exception(Exception::Id::APRS_TELEMETRY,
                    "DigitalParameter must be digital.");
  }
}

}; // namespace signal_easel::aprs::telemetry