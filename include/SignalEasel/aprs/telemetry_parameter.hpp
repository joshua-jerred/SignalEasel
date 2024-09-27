/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation
 * https://signaleasel.joshuajer.red/
 * https://github.com/joshua-jerred/SignalEasel
 * =*=======================*=
 * @file   telemetry_parameter.hpp
 * @date       2024-08-17
 * @copyright  2024
 * @license    {license}
 */

#pragma once

#include <cstdint>
#include <string>

#include <SignalEasel/exception.hpp>

namespace signal_easel::aprs::telemetry {

/// @brief A representation of APRS telemetry parameters.
class Parameter {
public:
  /// @brief The ID of the telemetry parameter.
  enum class Id : uint8_t {
    A1 = 0,
    A2 = 1,
    A3 = 2,
    A4 = 3,
    A5 = 4,
    B1 = 5,
    B2 = 6,
    B3 = 7,
    B4 = 8,
    B5 = 9,
    B6 = 10,
    B7 = 11,
    B8 = 12
  };

  /// @brief The type of the telemetry parameter.
  enum class Type : uint8_t { Analog = 0, Digital = 1 };

  /// @brief Get the type of the parameter.
  /// @param id - The ID of the parameter.
  /// @return \c Type::Analog if the parameter is analog, \c Type::Digital if
  /// the parameter is digital.
  static Type getType(Id id) {
    switch (id) {
    case Id::A1:
    case Id::A2:
    case Id::A3:
    case Id::A4:
    case Id::A5:
      return Type::Analog;
    case Id::B1:
    case Id::B2:
    case Id::B3:
    case Id::B4:
    case Id::B5:
    case Id::B6:
    case Id::B7:
    case Id::B8:
      return Type::Digital;
    };

    throw Exception(Exception::Id::APRS_TELEMETRY, "Invalid parameter ID");
  }

  /// @brief Get the parameter ID.
  /// @return Parameter ID.
  Id getId() const { return id_; }

  /// @brief Get the parameter type.
  /// @return Parameter type.
  Type getType() const { return type_; }

  /// @brief Set the name of the parameter.
  /// @param name - The name of the parameter.
  /// @return \c true if the name was set (in bounds), \c false otherwise.
  bool setName(const std::string &name);

  /// @brief Get the name of the parameter.
  /// @return The name of the parameter.
  std::string getName() const { return name_; }

  /// @brief Set the unit or label of the parameter.
  /// @param unit_or_label - The unit or label of the parameter.
  /// @return \c true if the unit or label was set (in bounds), \c false
  /// otherwise.
  bool setUnitOrLabel(const std::string &unit_or_label);

  /// @brief Get the unit or label of the parameter.
  /// @return The unit or label of the parameter.
  std::string getLabel() const { return unit_or_label_; }

protected:
  /// @brief Construct a new Parameter with the given ID.
  /// @param id - The ID of the parameter.
  /// @param type - The type of the parameter.
  Parameter(Id id, Type type);

  /// @brief Default destructor.
  ~Parameter() = default;

private:
  /// @brief Parameter names, along with units for analog parameters and labels
  /// for digital parameters, share the same constraints. This will validate
  /// these values dependent on their ID.
  /// @param descriptor - The name, unit, or label of the parameter.
  /// @return \c true if the descriptor is valid, \c false otherwise.
  bool validateParameterDescriptor(const std::string &descriptor);

  const Id id_;
  const Type type_;

  std::string name_{};
  std::string unit_or_label_{};
};

class AnalogParameter : public Parameter {
public:
  /// @brief An analog parameter.
  /// @param id - The ID of the parameter, must be AX.
  AnalogParameter(Id id);

  /// @brief Set the raw value of the parameter. 0-255.
  /// @param raw_value - The raw value of the parameter.
  void setRawValue(uint8_t raw_value) { raw_value_ = raw_value; }

  /// @brief Get the raw value of the parameter.
  /// @return The raw value.
  uint8_t getValue() const { return raw_value_; }

  /// @brief Get the calculated value of the parameter using the formula
  /// y = ax^2 + bx + c.
  /// @return The calculated value.
  double getCalculatedValue() const;

  /// @brief Set the A coefficient in the formula y = ax^2 + bx + c.
  /// @param coefficient - The A coefficient.
  /// @return \c true if the coefficient was set, \c false otherwise.
  bool setCoefficientA(const std::string &coefficient);

  /// @brief Get the A coefficient in the formula y = ax^2 + bx + c.
  /// @return The A coefficient.
  std::string getCoefficientA() const { return coefficient_a_; }

  /// @brief Set the B coefficient in the formula y = ax^2 + bx + c.
  /// @param coefficient - The B coefficient.
  /// @return \c true if the coefficient was set, \c false otherwise.
  bool setCoefficientB(const std::string &coefficient);

  /// @brief Get the B coefficient in the formula y = ax^2 + bx + c.
  /// @return The B coefficient.
  std::string getCoefficientB() const { return coefficient_b_; }

  /// @brief Set the C coefficient in the formula y = ax^2 + bx + c.
  /// @param coefficient - The C coefficient.
  /// @return \c true if the coefficient was set, \c false otherwise.
  bool setCoefficientC(const std::string &coefficient);

  /// @brief Get the C coefficient in the formula y = ax^2 + bx + c.
  /// @return The C coefficient.
  std::string getCoefficientC() const { return coefficient_c_; }

  /// @brief Set all coefficients in the formula y = ax^2 + bx + c.
  /// @param coefficient_a - The a coefficient.
  /// @param coefficient_b - The b coefficient.
  /// @param coefficient_c - The c coefficient.
  /// @return \c true if all coefficients were set, \c false otherwise.
  bool setCoefficients(const std::string &coefficient_a,
                       const std::string &coefficient_b,
                       const std::string &coefficient_c);

  /// @brief Get all coefficients in the formula y = ax^2 + bx + c.
  /// @param[out] coefficient_a - The a coefficient.
  /// @param[out] coefficient_b - The b coefficient.
  /// @param[out] coefficient_c - The c coefficient.
  void getCoefficients(std::string &coefficient_a, std::string &coefficient_b,
                       std::string &coefficient_c) const {
    coefficient_a = coefficient_a_;
    coefficient_b = coefficient_b_;
    coefficient_c = coefficient_c_;
  }

private:
  /// @brief The raw value of the analog parameter (0-255).
  uint8_t raw_value_{0};

  /// @brief The a coefficient in the formula y = ax^2 + bx + c
  std::string coefficient_a_{"0"};
  /// @brief The b coefficient in the formula y = ax^2 + bx + c
  std::string coefficient_b_{"1"};
  /// @brief The c coefficient in the formula y = ax^2 + bx + c
  std::string coefficient_c_{"0"};

  /// @brief The numerical value of the a coefficient.
  double a_{0.0};
  /// @brief The numerical value of the b coefficient.
  double b_{1.0};
  /// @brief The numerical value of the c coefficient.
  double c_{0.0};
};

/// @brief A digital parameter.
class DigitalParameter : public Parameter {
public:
  /// @brief Construct a new DigitalParameter with the given ID.
  DigitalParameter(Id id);

  /// @brief Set the value of the parameter.
  /// @param value - The value of the parameter.
  void setValue(bool value) { value_ = value; }

  /// @brief Get the value of the parameter.
  /// @return The value of the parameter.
  bool getValue() const { return value_; }

  /// @brief Set the bit sense of the parameter. If the value is true, but the
  /// bit sense is false, whatever the label is is considered false.
  void setBitSense(bool bit_sense) { bit_sense_ = bit_sense; }

  /// @brief Get the bit sense of the parameter.
  bool getBitSense() const { return bit_sense_; }

private:
  /// @brief The value of the digital parameter.
  bool value_{false};

  /// @brief The bit sense of the digital parameter.
  bool bit_sense_{true};
};

/// @brief Validate the format of a coefficient.
/// @param coefficient - The coefficient to validate.
/// @param[out] numerical_value - The numerical value of the coefficient. This
/// is only set if the coefficient is valid.
/// @return \c true if the coefficient is valid, \c false otherwise.
bool validateCoefficient(const std::string &coefficient,
                         double &numerical_value);

/// @brief Wrapper for validateCoefficient that does not require the numerical
/// value.
/// @param coefficient - The coefficient to validate.
/// @return \c true if the coefficient is valid, \c false otherwise.
bool validateCoefficient(const std::string &coefficient);

}; // namespace signal_easel::aprs::telemetry